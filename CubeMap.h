#pragma once

struct CubeMap : public Texture {
	
	//Load the cubemap in as 6 seperate image files: x -x y -y z -z
	CubeMap(char** images, ILenum origin=IL_ORIGIN_LOWER_LEFT):Texture(GL_TEXTURE_CUBE_MAP){

		ilEnable(IL_ORIGIN_SET);
		ilOriginFunc(origin);

		Texture::bind();

		//ILuint width, height;
		unsigned char* buffer;
		for (int i=0; i < 6; i++){
			ILuint id;
			ilGenImages(1, &id);
			ilBindImage(id);
			ilLoadImage(images[i]);
			printDevILErrors();

			if (i == 0){
				int imageWidth = ilGetInteger( IL_IMAGE_WIDTH )
					,imageHeight = ilGetInteger( IL_IMAGE_HEIGHT);
			
				width = ceilPow2(imageWidth);
				height = ceilPow2(imageHeight);
				width = height = std::max(width, height);
				buffer = new unsigned char[4*width*height];
			}

			iluScale(width, height, 1);
			ilCopyPixels(0, 0, 0, width, height, 1, IL_RGBA, IL_UNSIGNED_BYTE, buffer);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X+i, 0, GL_RGBA, width, height, 0, GL_RGBA, IL_UNSIGNED_BYTE, buffer);
			ilDeleteImages  (1, &id);
		}
		delete [] buffer;

		//use mipmaps so blurry reflections can be sampled from from them
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	}

	/************************************************************************************************************
	The following functions can be used to generate a cubemap based on the actual scene
	several changes made to this since I tested it last so there is probably a bug but the main idea works
	************************************************************************************************************/
	CubeMap(ILuint dim):Texture(GL_TEXTURE_CUBE_MAP){
		printGLErrors("begin cubemap constructor");
		width = height = dim;
	
		Texture::bind();

		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X+0, 0, GL_RGBA, dim, dim, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X+1, 0, GL_RGBA, dim, dim, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X+2, 0, GL_RGBA, dim, dim, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X+3, 0, GL_RGBA, dim, dim, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X+4, 0, GL_RGBA, dim, dim, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X+5, 0, GL_RGBA, dim, dim, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);//probably not needed
	}

	void update(vec3 pos, float nearPlane=.1f, float farPlane=1000.f){
		printGLErrors("cubemap update");
		Framebuffer::instance().bind();
		Viewport::push(0,0,width,height);

		//lookAts found with guess and check because I suck at finding shit online
		//x
		Scene::current->pushEye(perspective(90.0f, 1.0f, nearPlane, farPlane), lookAt(pos, pos+vec3(1,0,0), vec3(0,-1,0)));
		Framebuffer::instance().attachCube(this, GL_TEXTURE_CUBE_MAP_POSITIVE_X);
		glClear(GL_DEPTH_BUFFER_BIT);
		Scene::current->draw();
	
		//-x
		Scene::current->setView(lookAt(pos, pos+vec3(-1,0,0), vec3(0,-1,0)));
		Framebuffer::instance().attachCube(this, GL_TEXTURE_CUBE_MAP_NEGATIVE_X);
		glClear(GL_DEPTH_BUFFER_BIT);
		Scene::current->draw();

		//y
		Scene::current->setView(lookAt(pos, pos+vec3(0,1,0), vec3(0,0,1)));
		Framebuffer::instance().attachCube(this, GL_TEXTURE_CUBE_MAP_POSITIVE_Y);
		glClear(GL_DEPTH_BUFFER_BIT);
		Scene::current->draw();
	
		//-y
		Scene::current->setView(lookAt(pos, pos+vec3(0,-1,0), vec3(0,0,-1)));
		Framebuffer::instance().attachCube(this, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y);
		glClear(GL_DEPTH_BUFFER_BIT);
		Scene::current->draw();
	
		//z
		Scene::current->setView(lookAt(pos, pos+vec3(0,0,1), vec3(0,-1,0)));
		Framebuffer::instance().attachCube(this, GL_TEXTURE_CUBE_MAP_POSITIVE_Z);
		glClear(GL_DEPTH_BUFFER_BIT);
		Scene::current->draw();
	
		//-z
		Scene::current->setView(lookAt(pos, pos+vec3(0,0,-1), vec3(0,-1,0)));
		Framebuffer::instance().attachCube(this, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z);
		glClear(GL_DEPTH_BUFFER_BIT);
		Scene::current->draw();

		Scene::current->popEye();
		Viewport::pop();
		Framebuffer::instance().unbind();

		generateMipmaps();
		printGLErrors("/cubemap update");
	}
};