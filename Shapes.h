#pragma once

struct Shapes {

	struct Triangle {
		vec3 vertices[3];
		vec3 operator [](size_t i){return vertices[i];}
	};

	static VAO* square(){
		static VAO* vao = NULL;
		if (vao) return vao;

		GLfloat vertices[] = {-1,-1, 1,-1, 1,1, -1,1};
		vec3 normals[] = {vec3(0,0,1),vec3(0,0,1),vec3(0,0,1),vec3(0,0,1)};
		GLfloat textCoords[] = {0,0, 1,0, 1,1, 0,1};
		vao = new VAO(GL_QUADS);
		vao->bind(4);
		vao->buffer(vertices, sizeof(vertices));
		vao->in(2, GL_FLOAT);
		vao->buffer(normals, sizeof(normals));
		vao->in(3, GL_FLOAT);
		vao->buffer(textCoords, sizeof(textCoords));
		vao->in(2, GL_FLOAT);
		vao->unbind();
		return vao;
	}

	static VAO* cube(){
		static VAO* vao = NULL;
		if (vao) return vao;
	
		float vertices[] = {
			//x
			.5,-.5,-.5,
			.5,.5,-.5,
			.5,.5,.5,
			.5,-.5,.5,
			//-x
			-.5,-.5,-.5,
			-.5,-.5,.5,
			-.5,.5,.5,
			-.5,.5,-.5,
			//y
			.5,.5,.5,
			.5,.5,-.5,
			-.5,.5,-.5,
			-.5,.5,.5,
			//-y
			.5,-.5,.5,
			-.5,-.5,.5,
			-.5,-.5,-.5,
			.5,-.5,-.5,
			//z
			.5,-.5,.5,
			.5,.5,.5,
			-.5,.5,.5,
			-.5,-.5,.5,
			//-z
			.5,-.5,-.5,
			-.5,-.5,-.5,
			-.5,.5,-.5,
			.5,.5,-.5
		};

		float normals[] = {
			1,0,0,
			1,0,0,
			1,0,0,
			1,0,0,

			-1,0,0,
			-1,0,0,
			-1,0,0,
			-1,0,0,

			0,1,0,
			0,1,0,
			0,1,0,
			0,1,0,

			0,-1,0,
			0,-1,0,
			0,-1,0,
			0,-1,0,

			0,0,1,
			0,0,1,
			0,0,1,
			0,0,1,

			0,0,-1,
			0,0,-1,
			0,0,-1,
			0,0,-1
		};
		/*
		float textCoords[] = {
			1,0,0,
			1,1,0,
			0,1,1,
			0,0,1,

			0,0,0,
			1,0,1,
			1,1,1,
			0,1,0,

			1,0,1,
			1,1,0,
			0,1,0,
			0,0,1,

			1,1,1,
			1,0,1,
			0,0,0,
			0,1,0,

			1,0,1,
			1,1,1,
			0,1,1,
			0,0,1,

			0,0,0,
			1,0,0,
			1,1,0,
			0,1,0
		};
		*/
		vao = new VAO(GL_QUADS);
		vao->bind(sizeof(vertices)/(3*sizeof(float)));
		vao->buffer(vertices, sizeof(vertices));
		vao->in(3, GL_FLOAT);
		vao->buffer(normals, sizeof(normals));
		vao->in(3, GL_FLOAT);
		//vao->buffer(textCoords, sizeof(textCoords));
		//vao->in(3, GL_FLOAT);
		vao->unbind();
		return vao;
	}

	static VAO* cube_background(){
		static VAO* vao = NULL;
		if (vao) return vao;
		
		GLfloat vertices[] = {
		  -1.0,  1.0,  1.0,
		  -1.0, -1.0,  1.0,
		   1.0, -1.0,  1.0,
		   1.0,  1.0,  1.0,
		  -1.0,  1.0, -1.0,
		  -1.0, -1.0, -1.0,
		   1.0, -1.0, -1.0,
		   1.0,  1.0, -1.0,
		};

		GLushort indices[] = {
			0, 1, 2, 3,
			3, 2, 6, 7,
			7, 6, 5, 4,
			4, 5, 1, 0,
			0, 3, 7, 4,
			1, 5, 6, 2
		};

		vao = new VAO(GL_QUADS);
		vao->bind(6*4);
		vao->buffer(vertices, sizeof(vertices));
		vao->in(3,GL_FLOAT);
		vao->buffer(indices, sizeof(indices));
		vao->indices();
		vao->unbind();
		return vao;
	}

	//sphere helper
	template <class T>
	struct v2_cmp {
		bool operator()(T v1, T v2){
			auto min1 = std::min(v1.x,v1.y);
			auto min2 = std::min(v2.x,v2.y);
			auto max1 = std::max(v1.x,v1.y);
			auto max2 = std::max(v2.x,v2.y);

			return min1 < min2 || min1 == min2 && max1 < max2;
		}
	};

	static VAO* sphere(int loops=3){
		static VAO* vao = NULL;
		if (vao) return vao;

		typedef GLushort I;
		GLenum GL_I = GL_UNSIGNED_SHORT;
		typedef glm::detail::tvec3<I> Triangle;
		typedef glm::detail::tvec2<I> I2;

		vector<vec3> vertices;
		vector<I> i1, i2;

		float startVerts[] = {
			-1,0,0
			,1,0,0
			,0,-1,0
			,0,1,0
			,0,0,-1
			,0,0,1
		};

		I startIndices[] = {
			0,5,3
			,0,2,5
			,1,3,5
			,1,5,2

			,0,3,4
			,0,4,2
			,1,2,4
			,1,4,3
		};

		vertices.assign((vec3*)startVerts, (vec3*)(startVerts + sizeof(startVerts) / sizeof(float)));
		i1.assign(startIndices, startIndices + sizeof(startIndices) / sizeof(I));

		map<I2,I,v2_cmp<I2>> midpts;
		vector<I> *from, *to;
		if (!loops) to = &i1;
		for (int i=0; i < loops; i++){
			from = i%2?&i2:&i1;
			to = i%2?&i1:&i2;

			while (from->size()){
				Triangle t = *((Triangle*)&*(from->end()-3));
				//vec3 midpt = normalize((t[0] + t[1] + t[2]) / 3.f); 
				I mp[3];
				#define V(i) vertices[i]
				for (int i=0; i < 3; i++){
					I2 pts(t[i],t[(i+1)%3]);
					auto m = midpts.find(pts);
					if (m == midpts.end()){
						mp[i] = vertices.size();
						vertices.push_back(normalize((V(pts[0]) + V(pts[1])) / 2.0f));
						midpts[pts] = mp[i];
					} else
						mp[i] = m->second;
				}
				#undef V

				Triangle t1(mp[0],mp[1],mp[2])
					,t2(t[0],mp[0],mp[2])
					,t3(t[1],mp[1],mp[0])
					,t4(t[2],mp[2],mp[1]);

				to->insert(to->end(),(I*)&t1,(I*)&t1+3);
				to->insert(to->end(),(I*)&t2,(I*)&t2+3);
				to->insert(to->end(),(I*)&t3,(I*)&t3+3);
				to->insert(to->end(),(I*)&t4,(I*)&t4+3);

				from->erase(from->end()-3,from->end());
			}
		}

		auto normals = computeNormals(vertices,*to,3);

		vao = new VAO(GL_TRIANGLES);
		vao->bind(to->size());
		vao->buffer(&vertices[0], vertices.size() * sizeof(vec3));
		vao->in(3,GL_FLOAT);
		vao->buffer(&normals[0], normals.size() * sizeof(vec3));
		vao->in(3,GL_FLOAT);
		vao->buffer(&(*to)[0], to->size() * sizeof(I));
		vao->indices(GL_I);
		vao->unbind();
		vao->boundingVolume = new BoundingSphere(vec3(0),1);
		return vao;
	}

	static VAO* file(const char* file){
		static map<const char*,VAO*> cache;
		auto i = cache.find(file);
		if (i != cache.end())
			return i->second;

		VAO* vao=NULL;
		if (getFileExtension(file) == "off")
			vao = OFF(file);
			
		if (vao != NULL){
			cache[file] = vao;
			return vao;
		}

		error("unknown file extension");
		return NULL;//don't let this happen
	}


	static VAO* OFF(const char* file){
		typedef GLuint I;
		GLenum GL_I = GL_UNSIGNED_INT;
		vector<vec3> vertices;
		vector<I> indices;
		::loadOFF(file,vertices,indices);

		auto normals = computeNormals(vertices,indices,3);
		
		auto vao = new VAO(GL_TRIANGLES);
		vao->bind(indices.size());
		vao->buffer(&vertices[0], vertices.size() * sizeof(vec3));
		vao->in(3,GL_FLOAT);
		vao->buffer(&normals[0], normals.size() * sizeof(vec3));
		vao->in(3,GL_FLOAT);
		vao->buffer(&indices[0], indices.size() * sizeof(I));
		vao->indices(GL_I);
		vao->unbind();
		vao->boundingVolume = new BoundingSphere(&vertices[0], vertices.size());
		return vao;
	}

	//wow obj stupid fucking format, fuck those assholes
	static void loadObj4(const char* file, vector<vec3>& vertices, vector<vec2>& texCoords)
	{
		vector<vec3> verts;
		vector<vec2> tcs;

		//hacked some shit code for this one specific .obj file fuck you
		FILE *fin;
		fin = fopen(file,"r");
		char line[200];
		unsigned int v1,v2,v3,v4,t1,t2,t3,t4,n1,n2,n3,n4; 
		while(fgets(line,200,fin)!=NULL)
		{
			if(line[0] == 'v' && line[1] == ' '){
				vec3 v;
				sscanf(line, "v %f %f %f",&v.x,&v.y,&v.z);
				verts.push_back(v);
				continue;
			}
			if(line[0] == 'v' && line[1] == 't'){
				vec2 v; 
				sscanf(line, "vt %f %f",&v.x,&v.y);
				tcs.push_back(v);
				continue;
			}
			if(line[0] == 'f'){
				sscanf(line, "f %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d",&v1,&t1,&n1,&v2,&t2,&n2,&v3,&t3,&n3,&v4,&t4,&n4);
				vertices.push_back(verts[v1-1]);
				vertices.push_back(verts[v2-1]);
				vertices.push_back(verts[v3-1]);
				vertices.push_back(verts[v4-1]);
			 
				texCoords.push_back(tcs[t1-1]);
				texCoords.push_back(tcs[t2-1]);
				texCoords.push_back(tcs[t3-1]);
				texCoords.push_back(tcs[t4-1]);
				continue;
			}
		}
		fclose(fin);
	}

	static VAO* OBJ4(const char* file){
		GLenum GL_I = GL_UNSIGNED_INT;
		vector<vec3> vertices;
		vector<vec2> texCoords;
		loadObj4(file,vertices,texCoords);

		auto vao = new VAO(GL_QUADS);
		vao->bind(vertices.size());
		vao->buffer(&vertices[0], vertices.size() * sizeof(vec3));
		vao->in(3,GL_FLOAT);
		vao->buffer(&texCoords[0], texCoords.size() * sizeof(vec2));
		vao->in(2,GL_FLOAT);
		vao->unbind();
		vao->boundingVolume = new BoundingSphere(&vertices[0], vertices.size());
		return vao;
	}
};