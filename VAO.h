#pragma once

struct VAO {
	GLuint vao;
	GLenum mode, hint;
	vector<GLuint> buffers;
	GLuint count;
	GLuint i;

	GLenum indiceType;

	//must be set from outside
	IBoundingVolume* boundingVolume;

	VAO(GLenum mode = GL_TRIANGLES, GLenum hint = GL_STATIC_DRAW):mode(mode), hint(hint), i(0), indiceType(0), boundingVolume(NULL){}
	
	void operator()(GLenum mode = GL_TRIANGLES, GLenum hint = GL_STATIC_DRAW){
		this->mode = mode; this->hint = hint;
	}

	void bind(GLuint count=0){
		this->count = count;
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);
	}

	GLuint buffer(void* buf, int size){
		GLuint gid;
		glGenBuffers(1, &gid);
		glBindBuffer(GL_ARRAY_BUFFER, gid);
		glBufferData(GL_ARRAY_BUFFER, size, buf, hint);
		buffers.push_back(gid);
		return gid;
	}

	void in(int elementSize, GLenum type, GLuint stride=0, GLuint start=0){
		glEnableVertexAttribArray(i);
		glVertexAttribPointer(i, elementSize, type, GL_FALSE, stride, (void*)start);
		i++;
	}

	//must come last
	void indices(GLenum type=GL_UNSIGNED_SHORT){
		indiceType = type;
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers.back());
	}

	void unbind(){
		glBindVertexArray(0);
	}
	
	void update(void* data, GLuint size, int buf=0, int start=0){
		glBindBuffer(GL_ARRAY_BUFFER, buffers[buf]);
		glBufferSubData(GL_ARRAY_BUFFER, start, size, data);
		printGLErrors("/update");
	}


	void draw(GLuint num=0, GLuint start=0, GLenum mode=-1){//GL_POINTS stole 0 :(
		printGLErrors("vao draw");
		if (!num)
			num = count;

		if (mode==-1)
			mode = this->mode;

		glBindVertexArray(vao);
		if (indiceType)
			glDrawElements(mode, num, indiceType, (void*)start);
		else
			glDrawArrays(mode, 0, num);
		glBindVertexArray(0);
		printGLErrors("vao /draw");
	}
};

//quick and dirty vector that keeps gpu buffer up to date
template <class T> struct GVector : public vector<T>, public VAO {
	GVector(GLenum mode = GL_TRIANGLES, GLenum hint = GL_STATIC_DRAW):vector<T>(),VAO(mode,hint){}

	void set(size_t i, T& v){
		(*this)[i] = v;
		push(i);
	}

	void erase(size_t i){
		vector<T>::erase(begin()+i);
		pushToEnd(i);
	}

	void push_back(T& v){
		vector<T>::push_back(v);
		push(vector<T>::size() - 1);
	}

	void insertBefore(size_t i, T& v){
		insert(begin()+i, v);
		pushToEnd(i);
	}

	void push(size_t i){
		update(&(*this)[i],sizeof(T), 0, i*sizeof(T));
	}

	void pushToEnd(size_t i){
		if (i < size())
			update(&(*this)[i],(size()-i)*sizeof(T), 0, i*sizeof(T));
	}

	void pushAll(){
		if (size())
			update(&(*this)[0],size() * sizeof(T));
	}

	void drawAll(GLenum mode=-1){
		if (size())
			draw(size(), 0, mode);
	}
};