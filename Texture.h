#pragma once

//hack to get around dependency loop
void draw(Texture* tex, float x, float y, float w, float h);
void bind2FB(Texture* tex);
void unbind2FB();


struct Texture {
	int width, height;
	GLuint gid;
	GLenum target;
	bool mipmaps;
	GLenum wrap;

	//used to make copies
	GLenum internalFormat;

	Texture(GLenum target=GL_TEXTURE_2D):target(target){
		glGenTextures(1, &gid);
	}

	//use this if not passing in any data
	Texture(int width, int height, GLenum internalFormat=GL_RGBA, bool mipmaps=false, GLenum wrap = GL_REPEAT):target(GL_TEXTURE_2D){
		glGenTextures(1, &gid);
		operator()(width, height, internalFormat, mipmaps, wrap);
	}

	//use this if not passing in any data
	void operator()(int width, int height, GLenum internalFormat=GL_RGBA, bool mipmaps=false, GLenum wrap = GL_REPEAT){
		//eliminate stupid driver bugs, will update as needed
		//even though these aren't used for anything, they still have to be compatible with internal format on nvidea
		GLenum format = GL_RGBA, type=GL_UNSIGNED_BYTE;
		switch(internalFormat){
			case GL_RGB32UI:
				format = GL_RGB_INTEGER;
				type = GL_UNSIGNED_INT;
				break;
		}

		operator()(NULL, width, height, internalFormat, format, type, mipmaps, wrap);
	}

	Texture(void* data, int width, int height, GLenum internalFormat=GL_RGBA, GLenum format=GL_RGBA, GLenum type=GL_UNSIGNED_BYTE, bool mipmaps=false, GLenum wrap = GL_REPEAT):target(GL_TEXTURE_2D){
		glGenTextures(1, &gid);
		operator()(data, width, height, internalFormat, format, type, mipmaps, wrap);
	}

	void operator()(void* data, int width, int height, GLenum internalFormat=GL_RGBA, GLenum format=GL_RGBA, GLenum type=GL_UNSIGNED_BYTE, bool mipmaps=false, GLenum wrap = GL_REPEAT){
		this->width = width;
		this->height = height;
		this->internalFormat = internalFormat;

		bind();
		glTexImage2D(target, 0, internalFormat, width, height, 0, format, type, data);
		sampleSettings(wrap, mipmaps);
	}

	void sampleSettings(GLenum wrap, bool mipmaps=true){
		this->wrap = wrap;
		this->mipmaps = mipmaps;

		if (wrap == 0 || wrap == 1)
			cout <<"sampleSettings warp & mipmaps pass in backwards" <<endl;//leave this in for a while
		glTexParameteri(target, GL_TEXTURE_WRAP_S, wrap);
		glTexParameteri (target, GL_TEXTURE_WRAP_T, wrap);
		if (target == GL_TEXTURE_3D) glTexParameteri(target, GL_TEXTURE_WRAP_R, wrap);

		glTexParameteri (target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		if (mipmaps){
			glTexParameteri (target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glGenerateMipmap(target);
		}
		else
			glTexParameteri (target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}

	//bind to unit 0 (0 is reserved for all operations)
	void bind(){
		activateUnit(0);
		static GLuint bound0 = 0;//might be more efficient if opengl is stupid
		if (bound0 != gid){
			glBindTexture(target, gid);
			bound0 = gid;
		}
	}

	void bind(GLuint unit){
		activateUnit(unit);
		glBindTexture(target, gid);
	}

	bool viewportPushed;
	void bind2FB(bool resizeViewport=true, bool clear=true){
		viewportPushed = resizeViewport;//store whether this needs popped
		if (resizeViewport) Viewport::push(0,0,width,height);
		::bind2FB(this);
		if (clear) glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void unbind2FB(bool genMipmaps=true){
		::unbind2FB();
		if (viewportPushed)
			Viewport::pop();
		if (mipmaps && genMipmaps)
			generateMipmaps();
	}

	void generateMipmaps(){
		bind();
		glGenerateMipmap(target);
	}

	//must use instead of glActiveTexture everywhere
	static void activateUnit(GLuint unit){
		static GLuint active = -1;//might be more efficient if opengl is stupid
		if (unit != active){
			glActiveTexture(GL_TEXTURE0 + unit);
			active = unit;
		}
	}

	static int maxUnits(){
		static int i=0;
		if (i) return i;
		glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &i);
		cout << i << " texture units" << endl;
		return i;
	}

	void draw(float x, float y, float w, float h){
		::draw(this, x, y, w, h);
	}

	void save(const char* file){
		size_t size = 3 * width * height;
		char* data = new char[size];
		bind();
		glGetTexImage(target, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

		if (data == NULL){
			msg("error saving","");
			return;
		}

		ILuint id;
		ilGenImages(1,&id);
		ilBindImage(id);
		ilTexImage(width,height,1,3,IL_RGB, IL_UNSIGNED_BYTE, data);
		ilEnable(IL_FILE_OVERWRITE);
		ilSaveImage(file);
	}
};

struct ILTexture : public Texture {

	ILTexture(const char* file, ILenum origin = IL_ORIGIN_LOWER_LEFT):Texture(GL_TEXTURE_2D){
		//generates, binds, loads, creates mipmaps, deletes local copy
		ilEnable(IL_ORIGIN_SET);
		ilOriginFunc(origin);

		ILuint id;
		ilGenImages(1, &id);
		ilBindImage(id);
		ilLoadImage(file);
		printDevILErrors();
		int imageWidth = ilGetInteger( IL_IMAGE_WIDTH )
			,imageHeight = ilGetInteger( IL_IMAGE_HEIGHT );

		width = ceilPow2(imageWidth);
		height = ceilPow2(imageHeight);
		width = height = std::min(4096,std::max(width, height));

		unsigned char* data = new unsigned char[4*width*height];
		iluScale(width, height, 1);
		ilCopyPixels(0, 0, 0, width, height, 1, GL_RGBA, IL_UNSIGNED_BYTE, data);
		operator()(data, width, height, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, true, GL_REPEAT);
		ilDeleteImages  (1, &id);
	}
};

struct Texture3D : public Texture {
	int depth;

	Texture3D():Texture(GL_TEXTURE_3D){}
#define PARAMS void* data, int width, int height, int depth, GLenum internalFormat=GL_RGBA, GLenum format=GL_RGBA, GLenum type=GL_UNSIGNED_BYTE, bool mipmaps=false, GLenum wrap=GL_CLAMP_TO_BORDER
	Texture3D(PARAMS):Texture(GL_TEXTURE_3D){
		operator()(data,width,height,depth,internalFormat,format,type,mipmaps,wrap);
	}

	void operator()(PARAMS){
		this->width = width;
		this->height = height;
		this->depth = depth;
		this->internalFormat = internalFormat;
		bind();
		glTexImage3D(target, 0, internalFormat, width, height, depth, 0, format, type, data);
		sampleSettings(wrap, mipmaps);
	}
#undef PARAMS
};


struct Texture1D : public Texture {

	Texture1D():Texture(GL_TEXTURE_1D){}
#define PARAMS void* data, int width, GLenum internalFormat=GL_RGBA, GLenum format=GL_RGBA, GLenum type=GL_UNSIGNED_BYTE, bool mipmaps=false, GLenum wrap=GL_REPEAT
	Texture1D(PARAMS):Texture(GL_TEXTURE_1D){
		operator()(data,width,internalFormat,format,type,mipmaps,wrap);
	}

	void operator()(PARAMS){
		this->width = width;
		this->internalFormat = internalFormat;

		bind();
		glTexImage1D(target, 0, internalFormat, width, 0, format, type, data);
		sampleSettings(wrap, mipmaps);
	}
#undef PARAMS
};