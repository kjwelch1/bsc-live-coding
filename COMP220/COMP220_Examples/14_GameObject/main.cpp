//main.cpp - defines the entry point of the application

#include "main.h"

int main(int argc, char* args[])
{
	//Initialises the SDL Library, passing in SDL_INIT_VIDEO to only initialise the video subsystems
	//https://wiki.libsdl.org/SDL_Init
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		//Display an error message box
		//https://wiki.libsdl.org/SDL_ShowSimpleMessageBox
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, SDL_GetError(), "SDL_Init failed", NULL);
		return 1;
	}

	//Create a window, note we have to free the pointer returned using the DestroyWindow Function
	//https://wiki.libsdl.org/SDL_CreateWindow
	SDL_Window* window = SDL_CreateWindow("SDL2 Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 800, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
	//Checks to see if the window has been created, the pointer will have a value of some kind
	if (window == nullptr)
	{
		//Show error
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, SDL_GetError(), "SDL_CreateWindow failed", NULL);
		//Close the SDL Library
		//https://wiki.libsdl.org/SDL_Quit
		SDL_Quit();
		return 1;
	}

	//lets ask for a 3.2 core profile version of OpenGL
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	SDL_GLContext GL_Context = SDL_GL_CreateContext(window);
	if (GL_Context == nullptr)
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, SDL_GetError(), "SDL GL Create Context failed", NULL);
		SDL_DestroyWindow(window);
		SDL_Quit();
		return 1;
	}
	//Initialize GLEW
	glewExperimental = GL_TRUE;
	GLenum glewError = glewInit();
	if (glewError != GLEW_OK)
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, (char*)glewGetErrorString(glewError), "GLEW Init Failed", NULL);
	}

	//Camera
	vec3 cameraPosition = vec3(0.0f, 2.0f, -15.0f);
	vec3 cameraTarget = vec3(0.0f, 0.0f, 0.0f);
	vec3 cameraUp = vec3(0.0f, 1.0f, 0.0f);

	mat4 viewMatrix = lookAt(cameraPosition, cameraTarget, cameraUp);
	mat4 projectionMatrix = perspective(radians(90.0f), float(800 / 800), 0.1f, 1000.0f);

	//Light
	vec4 ambientLightColour = vec4(1.0f, 1.0f, 1.0f, 1.0f);
	vec3 lightDirection = vec3(0.0f, 0.0f, -1.0f);
	vec4 diffuseLightColour = vec4(1.0f, 1.0f, 1.0f, 1.0f);
	vec4 specularLightColour = vec4(1.0f, 1.0f, 1.0f, 1.0f);


	std::vector<GameObject*> gameObjectList;

	//Create GameObject
	GameObject * pCar = new GameObject();
	pCar->loadMeshesFromFile("armoredrecon.fbx");
	pCar->loadDiffuseTextureFromFile("armoredrecon_diff.png");
	pCar->loadShaderProgram("textureVert.glsl", "textureFrag.glsl");
	gameObjectList.push_back(pCar);

	pCar = new GameObject();
	pCar->setPosition(vec3(6.0f, 0.0f, 0.0f));
	pCar->loadMeshesFromFile("Tank1.FBX");
	pCar->loadDiffuseTextureFromFile("Tank1DF.png");
	pCar->loadShaderProgram("textureVert.glsl", "textureFrag.glsl");
	gameObjectList.push_back(pCar);

	int zero = 0;
	int mouseX = 0;
	int mouseY = 0;
	float mouseSpeed = 0.0001f;
	float horizontalAngle = 0.0f;
	float verticalAngle = 0.0f;
	float deltaTime = 0.0f;
	float speed = 0.5f;

	//Colour Buffer Texture
	GLuint colourBufferID = createTexture(800, 800);

	//Create Depth Buffer
	GLuint depthRenderBufferID;
	glGenRenderbuffers(1, &depthRenderBufferID);
	glBindRenderbuffer(GL_RENDERBUFFER, depthRenderBufferID);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, 800, 800);

	//Create Framebuffer
	GLuint frameBufferID;
	glGenFramebuffers(1, &frameBufferID);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBufferID);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderBufferID);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, colourBufferID, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Unable to create frame buffer for post processing", "Frame Buffer Error", NULL);
	}

	//create screen aligned quad
	GLfloat screenVerts[] =
	{
		-1, -1,
		1, -1,
		-1, 1,
		1, 1
	};

	GLuint screenQuadVBOID;
	glGenBuffers(1, &screenQuadVBOID);
	glBindBuffer(GL_ARRAY_BUFFER, screenQuadVBOID);
	glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(GLfloat), screenVerts, GL_STATIC_DRAW);

	GLuint screenVAO;
	glGenVertexArrays(1, &screenVAO);
	glBindVertexArray(screenVAO);
	glBindBuffer(GL_ARRAY_BUFFER, screenQuadVBOID);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);

	GLuint postProcessingProgramID = LoadShaders("passThroughVert.glsl", "postBlackAndWhite.glsl");
	GLint texture0Location = glGetUniformLocation(postProcessingProgramID, "texture0");

	GLint programID = LoadShaders("lightingVert.glsl", "lightingFrag.glsl");
	
	GLint currentTimeLocation= glGetUniformLocation(programID, "time");
	GLint modelMatrixLocation = glGetUniformLocation(programID, "modelMatrix");
	GLint viewMatrixLocation = glGetUniformLocation(programID, "viewMatrix");
	GLint projectionMatrixLocation = glGetUniformLocation(programID, "projectionMatrix");
	GLint textureLocation = glGetUniformLocation(programID, "baseTexture");
	GLint cameraPositionLocation = glGetUniformLocation(programID, "cameraPosition");
	
	GLint lightDirectionLocation = glGetUniformLocation(programID, "lightDirection");
	GLint ambientLightColourLocation = glGetUniformLocation(programID, "ambientLightColour");
	GLint diffuseLightColourLocation = glGetUniformLocation(programID, "diffuseLightColour");
	GLint specularLightColourLocation = glGetUniformLocation(programID, "specularLightColour");

	GLint ambientMaterialColourLocation = glGetUniformLocation(programID, "ambientMaterialColour");
	GLint diffuseMaterialColourLocation = glGetUniformLocation(programID, "diffuseMaterialColour");
	GLint specularMaterialColourLocation= glGetUniformLocation(programID, "specularMaterialColour");
	GLint specularPowerLocation = glGetUniformLocation(programID, "specularPower");
	/*
	///collision configuration contains default setup for memory, collision setup. Advanced users can create their own configuration.
	btDefaultCollisionConfiguration* collisionConfiguration = new btDefaultCollisionConfiguration();

	///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
	btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collisionConfiguration);

	///btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
	btBroadphaseInterface* overlappingPairCache = new btDbvtBroadphase();

	///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
	btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver;

	btDiscreteDynamicsWorld* dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);

	dynamicsWorld->setGravity(btVector3(0, -10, 0));

	btCollisionShape* groundShape = new btBoxShape(btVector3(btScalar(50.), btScalar(2.), btScalar(50.)));

	btTransform groundTransform;
	groundTransform.setIdentity();
	groundTransform.setOrigin(btVector3(0, -10, 0));

	btScalar mass(0.);
	btVector3 localInertia(0, 0, 0);
	//using motionstate is optional, it provides interpolation capabilities, and only synchronizes 'active' objects
	btDefaultMotionState* myMotionState = new btDefaultMotionState(groundTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, groundShape, localInertia);
	btRigidBody* groundRigidBody = new btRigidBody(rbInfo);
	//add the body to the dynamics world
	dynamicsWorld->addRigidBody(groundRigidBody);


	btCollisionShape* carCollisionShape = new btBoxShape(btVector3(2, 2, 2));
	//Create Dynamic Objects
	btTransform carTransform;
	carTransform.setIdentity();
	carTransform.setOrigin(btVector3(objectPosition.x, objectPosition.y, objectPosition.z));
	btVector3 carInertia(0, 0, 0);
	btScalar carMass(1.f);
	carCollisionShape->calculateLocalInertia(carMass, carInertia);

	btDefaultMotionState* carMotionState = new btDefaultMotionState(carTransform);
	btRigidBody::btRigidBodyConstructionInfo carRbInfo(carMass, carMotionState, carCollisionShape, carInertia);
	btRigidBody* carRigidBody = new btRigidBody(carRbInfo);

	dynamicsWorld->addRigidBody(carRigidBody);*/
	//angle calculation
	glm::vec3 direction = glm::vec3();
	//right vector
	glm::vec3 right = glm::vec3();

	glEnable(GL_DEPTH_TEST);

	int lastTicks = SDL_GetTicks();
	int currentTicks = SDL_GetTicks();

	//Event loop, we will loop until running is set to false, usually if escape has been pressed or window is closed
	bool running = true;
	//SDL Event structure, this will be checked in the while loop
	SDL_Event ev;
	
	while (running)
	{
		//Poll for the events which have happened in this frame
		//https://wiki.libsdl.org/SDL_PollEvent
		while (SDL_PollEvent(&ev))
		{
			currentTicks = SDL_GetTicks();
			SDL_WarpMouseInWindow(window, 400, 400);
			deltaTime = (float)(currentTicks - lastTicks) / 1000.0f;
			lastTicks = currentTicks;
			horizontalAngle += mouseSpeed * deltaTime * float(800 / 2 - mouseX);
			verticalAngle += mouseSpeed * deltaTime * float(800 / 2 - mouseY);

			direction = glm::vec3(
				cos(verticalAngle) * sin(horizontalAngle),
				sin(verticalAngle),
				cos(verticalAngle) * cos(horizontalAngle)
			);
			direction = glm::normalize(direction);
			right = glm::vec3(
				sin(horizontalAngle - 3.14f / 2.0f),
				0,
				cos(horizontalAngle - 3.14f / 2.0f)
			);
			right = glm::normalize(right);
			// Up vector : perpendicular to both direction and right
			glm::vec3 up = glm::cross(right, direction);
			up = glm::normalize(up);
			viewMatrix = glm::lookAt(
				cameraPosition,           // Camera is here
				cameraPosition + direction, // and looks here : at the same position, plus "direction"
				up                  // Head is up (set to 0,-1,0 to look upside-down)
			);
			//Switch case for every message we are intereted in
			switch (ev.type)
			{
				//QUIT Message, usually called when the window has been closed
			case SDL_QUIT:
				running = false;
				break;
				//KEYDOWN Message, called when a key has been pressed down
			case SDL_KEYDOWN:
				//Check the actual key code of the key that has been pressed
				switch (ev.key.keysym.sym)
				{
					//Escape key
				case SDLK_ESCAPE:
					running = false;
					break;
				case SDLK_w:
					cameraPosition += direction * deltaTime * speed;
					break;
				case SDLK_s:
					cameraPosition -= direction * deltaTime * speed;
					break;
				case SDLK_d:
					cameraPosition += right * deltaTime * speed;
					break;
				case SDLK_a:
					cameraPosition -= right * deltaTime * speed;
					break;
				}

			case SDL_MOUSEMOTION:
				mouseX = ev.motion.x;
				mouseY = ev.motion.y;
				break;
			}
			currentTicks = 0;
			lastTicks = 0;
			/*SDL_WarpMouseInWindow(window, 400, 320);
			currentTicks = SDL_GetTicks();
			deltaTime = (float)(currentTicks - lastTicks) / 1000.0f;
			horizontalAngle += mouseSpeed * deltaTime * float(800 / 2 - mouseX);
			verticalAngle += mouseSpeed * deltaTime * float(640 / 2 - mouseY);

			direction = glm::vec3(
				cos(verticalAngle) * sin(horizontalAngle),
				sin(verticalAngle),
				cos(verticalAngle) * cos(horizontalAngle)
			);
			//direction = glm::normalize(direction);
			right = glm::vec3(
				sin(horizontalAngle - 3.14f / 2.0f),
				0,
				cos(horizontalAngle - 3.14f / 2.0f)
			);
			// Up vector : perpendicular to both direction and right
			glm::vec3 up = glm::cross(right, direction);

			viewMatrix = glm::lookAt(
				cameraPosition,           // Camera is here
				cameraPosition + direction, // and looks here : at the same position, plus "direction"
				up                  // Head is up (set to 0,-1,0 to look upside-down)
			);*/

		}
		
		for (GameObject * pObj : gameObjectList)
		{
			pObj->update();
		}
		//dynamicsWorld->stepSimulation(1.f / 60.f, 10);

		//carTransform=carRigidBody->getWorldTransform();
		//btVector3 carOrigin= carTransform.getOrigin();
		//btQuaternion carRotation = carTransform.getRotation();

		
		glEnable(GL_DEPTH_TEST);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClearDepth(1.0f);
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

		for (GameObject * pObj : gameObjectList)
		{
			pObj->preRender();
			GLuint currentProgramID = pObj->getShaderPorgramID();

			//retrieve the shader values
			GLint viewMatrixLocation = glGetUniformLocation(currentProgramID, "viewMatrix");
			GLint projectionMatrixLocation = glGetUniformLocation(currentProgramID, "projectionMatrix");
			GLint lightDirectionLocation = glGetUniformLocation(currentProgramID, "lightDirection");
			GLint ambientLightColourLocation = glGetUniformLocation(currentProgramID, "ambientLightColour");
			GLint diffuseLightColourLocation = glGetUniformLocation(currentProgramID, "diffuseLightColour");
			GLint specularLightColourLocation = glGetUniformLocation(currentProgramID, "specularLightColour");

			//send shader values
			glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, value_ptr(viewMatrix));
			glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, value_ptr(projectionMatrix));

			glUniform3fv(lightDirectionLocation, 1, value_ptr(lightDirection));
			glUniform4fv(ambientLightColourLocation, 1, value_ptr(ambientLightColour));
			glUniform4fv(diffuseLightColourLocation, 1, value_ptr(diffuseLightColour));
			glUniform4fv(specularLightColourLocation, 1, value_ptr(specularLightColour));

			pObj->render();
		}


		/*
		glUniform4fv(fragColourLocation, 1, fragColour);
		glUniform1f(currentTimeLocation, (float)(currentTicks)/1000.0f);
		glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, value_ptr(modelMatrix));
		glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, value_ptr(viewMatrix));
		glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, value_ptr(projectionMatrix));

		glUniform3fv(cameraPositionLocation, 1, value_ptr(cameraPosition));

		glUniform1i(textureLocation, 0);



		glUniform4fv(ambientMaterialColourLocation, 1, value_ptr(ambientMaterialColour));
		glUniform4fv(diffuseMaterialColourLocation, 1, value_ptr(diffuseMaterialColour));
		glUniform4fv(specularMaterialColourLocation, 1, value_ptr(specularMaterialColour));
		glUniform1f(specularPowerLocation, specularPower);
		*/

		//Draw stuff here
		/*
		glDisable(GL_DEPTH_TEST);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Bind Post Processing Shaders
		glUseProgram(postProcessingProgramID);

		//Activate texture unit 0 for the colour buffer
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, colourBufferID);
		glUniform1i(texture0Location, 0);

		glBindVertexArray(screenVAO);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		*/
		SDL_GL_SwapWindow(window);

		//lastTicks = currentTicks;
	}
	/**
	dynamicsWorld->removeRigidBody(carRigidBody);
	delete carCollisionShape;
	delete carRigidBody->getMotionState();
	delete carRigidBody;

	dynamicsWorld->removeRigidBody(groundRigidBody);

	//delete ground
	delete groundShape;
	delete groundRigidBody->getMotionState();
	delete groundRigidBody;

	//delete dynamics world
	delete dynamicsWorld;

	//delete solver
	delete solver;

	//delete broadphase
	delete overlappingPairCache;

	//delete dispatcher
	delete dispatcher;

	delete collisionConfiguration;*/

	auto gameObjectIter = gameObjectList.begin();
	while (gameObjectIter != gameObjectList.end())
	{
		if ((*gameObjectIter))
		{
			(*gameObjectIter)->destroy();
			delete (*gameObjectIter);
			gameObjectIter=gameObjectList.erase(gameObjectIter);
		}
	}
	glDeleteProgram(postProcessingProgramID);
	glDeleteVertexArrays(1, &screenVAO);
	glDeleteBuffers(1, &screenQuadVBOID);
	glDeleteFramebuffers(1, &frameBufferID);
	glDeleteRenderbuffers(1, &depthRenderBufferID);
	glDeleteTextures(1, &colourBufferID);

	SDL_GL_DeleteContext(GL_Context);
	//Destroy the window and quit SDL2, NB we should do this after all cleanup in this order!!!
	//https://wiki.libsdl.org/SDL_DestroyWindow
	SDL_DestroyWindow(window);
	//https://wiki.libsdl.org/SDL_Quit
	SDL_Quit();

	return 0;
}
