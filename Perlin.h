struct Perlin3D : public Texture3D {
	Perlin3D(int res, float freq=1){
		float* data = new float[res*res*res];
		int bytes = res*res*res*sizeof(float);
		//why the fuck is perlin func so slow..., just cache the texture on harddrive
		string file = getFileName(res,freq);
		fstream in(file, ios::in | ios::binary);
		if (in.is_open())
			in.read((char*)data, bytes);
		else {
			for (int i=0; i < res; i++)
				for (int j=0; j < res; j++)
					for (int k=0; k < res; k++)
						data[i*res*res + j*res + k] = perlin(vec3(i/(float)res,j/(float)res,k/(float)res)*freq);
		
			ofstream out(file, ios::out | ios::binary);
			if (out.is_open())
				out.write((char*)data, bytes);
			else cout << "can't save perlin texture for some reason" << endl;
		}

		operator()(data,res,res,res,GL_R32F,GL_RED,GL_FLOAT,false,GL_REPEAT);
		delete [] data;
	}

	string getFileName(int res, float freq){
		return string("perlin3D-") + itos(res) + "-" + itos(freq);
	}
};

struct Perlin2D : public Texture {
	Perlin2D(int res, float freq=1){
		float* data = new float[res*res];
		int bytes = res*res*sizeof(float);
		//why the fuck is perlin func so slow..., just cache the texture on harddrive
		string file = getFileName(res,freq);
		fstream in(file, ios::in | ios::binary);
		if (in.is_open())
			in.read((char*)data, bytes);
		else {
			for (int i=0; i < res; i++)
				for (int j=0; j < res; j++)
					data[i*res + j] = perlin(vec2(i/(float)res,j/(float)res)*freq);
		
			ofstream out(file, ios::out | ios::binary);
			if (out.is_open())
				out.write((char*)data, bytes);
			else cout << "can't save perlin texture for some reason" << endl;
		}

		operator()(data,res,res,GL_R32F,GL_RED,GL_FLOAT,false,GL_REPEAT);
		delete [] data;
	}

	string getFileName(int res, float freq){
		return string("perlin2D-") + itos(res) + "-" + itos(freq);
	}
};