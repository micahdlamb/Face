#pragma once
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include "glm/glm.hpp"

//replace with funcs eventually
#define msgVec3(p,m) cout<<p<<":"<<m.x<<","<<m.y<<","<<m.z<<endl;
#define choose3(a,b,c,d) (a==0?b:(a==1?c:d))
#define choose6(a,b,c,d,e,f,g) (a==0?b:(a==1?c:(a==2?d:a==3?e:(a==4?f:g))))
#define rand0_1 (rand()/(float)RAND_MAX)
#define NaN(x) (x!=x)//because std and glm versions are glitched
#define PI 3.14159265
#define RAD(x) ((x)*PI/180)
#define DEG(x) ((x)*180/PI)
typedef size_t U;
typedef unsigned char uchar;

using namespace std;
using namespace glm;

//in .cc file
string getFileNameWithoutExtension(string path);
string getFileExtension(string path);
bool solveQuadratic(float &s1, float &s2, float a, float b, float c);
vec2 ndCoord(vec2 pt,vec2 dim);
int ceilPow2(int val);
vec3 rainbowColors(float frequency=.005f);
string loadFile(string fname);
void printDevILErrors();
void printGLErrors(const char* m);

template <class T, class U>
void msg(T p, U m){cout << p << ":" << m << endl;}
template <class T, class U>
void msg(T m){cout << m << endl;}

inline void pause(){
	string s;
	cin >> s;
}

inline void error(string e){
	throw e;
}

template <class T>
T max3(T a, T b, T c){
	return (a >= b && a >= c) ? a : (b >= a && b >= c ? b : c)
}

template <class T>
string itos(T i){
	stringstream ss;
	ss << i;
	return ss.str();
}

template <class T>
T mod(T n, T m){
	T r = n % m;
	return r < 0 ? r + m : r;
}

template <class T>
bool contains(vector<T> v, T t){
	for (U i=0; i < v.size(); i++)
		if (v[i] == t) return true;
	return false;
}

template <class T>
void concat(vector<T>& v1, vector<T>& v2){
	/*
	vector<T> v = v1;
	v.insert(v.end(), v2.begin(), v2.end());
	return v;
	*/
	v1.insert(v1.end(), v2.begin(), v2.end());
}

template<class T>
U indexOf(vector<T>& vec, T& val){
	for (U i=0; i < vec.size(); ++i)
		if (vec[i] == val)
			return i;
	return -1;
}

template <class T> struct Array2d {
	U cols, rows;
	T *v;
	Array2d():v(NULL),cols(0),rows(0){}
	Array2d(const char* file):v(NULL){operator()(file);}
	Array2d(U cols, U rows):v(NULL){operator()(cols,rows);}
	void operator()(const char* file){load(file);}
	void operator()(U cols, U rows){
		this->cols = cols;
		this->rows = rows;
		delete [] v;
		v = new T[cols*rows];
	}

	T* operator [](U c){
		assert(c < cols);
		return v+rows*c;
	}

	//auto wrap
	/*
	T operator()(int c, int r){
		return *this[::mod(c,(int)cols)][::mod(r,(int)rows)];
	}
	*/
	U index(U c, U r){
		return rows*c+r;
	}
	U size() const{
		return rows*cols;
	}

	U bytes() const{
		return size() * sizeof(T);
	}

	T* value_ptr(){
		return v;
	}
	
	bool load(const char* file){
		fstream in(file, ios::in | ios::out | ios::binary);
		if (!in.is_open())
			return false;
		in.read((char*)&cols,sizeof(cols));
		in.read((char*)&rows,sizeof(rows));
		operator()(cols,rows);
		in.read((char*)v,rows*cols*sizeof(T));
		return true;
	}
	bool write(const  char* file){
		ofstream out(file, ios::out | ios::binary);
		if (!out.is_open())
			return false;
		out.write((char*)&cols, sizeof(cols));
		out.write((char*)&rows,sizeof(rows));
		out.write((char*)v,rows*cols*sizeof(T));
		out.close();
		return true;
	}

	//copy
	Array2d(const Array2d &a):v(NULL){
		operator()(a.cols, a.rows);
		memcpy(v,a.v,a.bytes());
	}

	//move
	Array2d(Array2d&& other){
		cols = other.cols;
		rows = other.rows;
		v = other.v;
		other.v = NULL;
	}

	//=
	Array2d& operator=(const Array2d& other){
		if (this != &other){
			operator()(other.cols, other.rows);
			memcpy(v,other.v,other.bytes());
		}
		return *this;
	}
	
	//move =
	Array2d& operator=(Array2d&& other){
		if (this != &other){
			delete [] v;
			cols = other.cols;
			rows = other.rows;
			v = other.v;
			other.v = NULL;
		}
		return *this;
	}

	//destroooy
	~Array2d(){
		delete [] v;
	}
};

template <class T>
bool load(const char* file, vector<T>& v){
	fstream in(file, ios::in | ios::binary);
	if (!in.is_open())
		return false;
	U size;
	in.read((char*)&size,sizeof(size));
	v.resize(size);
	in.read((char*)&v[0],size*sizeof(T));
	in.close();
	return true;
}


template <class T>
bool write(const char* file, vector<T>& v){
	fstream out(file, ios::out | ios::binary);
	if (!out.is_open())
		return false;
	U size = v.size();
	out.write((char*)&size,sizeof(size));
	out.write((char*)&v[0],size*sizeof(T));
	out.close();
	return true;
}

template <class T>
T biLinearInterpolate(T bl, T br, T tl, T tr, float hfract, float vfract){
	T bot = bl*(1-hfract) + br * hfract
		,top = tl*(1-hfract) + tr * hfract;

	return bot*(1-vfract) + top * vfract;
}

template <class T>
vector<vec3> computeNormals(const vector<vec3>& vertices, const vector<T>& indices, int size){
	vector<vec3> r;
	r.resize(vertices.size());//should be zeroed automatically
	//#pragma omp parallel for
	for (int i=0; i < (int)indices.size(); i+=size){
		vec3 v1 = vertices[indices[i+1]] - vertices[indices[i]]
			,v2 = vertices[indices[i+2]] - vertices[indices[i]];
		vec3 n = normalize(cross(v1,v2));
		for (int j=0; j < size; j++)
			r[indices[i+j]] += n;//race condition only noticeable when number of faces is small (just started recently to wtf....)
	}

	#pragma omp parallel for
	for (int i=0; i < (int)r.size(); i++)
		r[i] = normalize(r[i]);
	
	return r;
}

template <class T>
Array2d<vec3> computeNormals(Array2d<T>& heights){
	Array2d<vec3> normals(heights.cols, heights.rows);
	vector<vec3> neighboors(4);
	//loop through how Array2d is actually located in memory
	#define VERT(x,y,c) vec3((float)(x)/(heights.cols-1), (c)?heights[x][y]:0, (float)(y)/(heights.rows-1))
	for (U x=0; x < heights.cols; x++){
		for (U y=0; y < heights.rows; y++){
			vec3 me = VERT(x,y,true)
				,n(0);
			neighboors.clear();
			neighboors.push_back(VERT((int)x-1,y,x!=0)-me);
			neighboors.push_back(VERT(x,(int)y-1,y!=0)-me);
			neighboors.push_back(VERT(x+1,y,x != heights.cols-1)-me);
			neighboors.push_back(VERT(x,y+1,y != heights.rows-1)-me);

			for (U i=0; i < neighboors.size()-1; i++)
				n += normalize(cross(neighboors[i+1], neighboors[i]));//not sure if needs normalized here

			if (NaN(n.x)||NaN(n.y)||NaN(n.z))
				cout <<"NaN normal"<<endl;
			normals[x][y] = normalize(n);
		}
	}
	#undef VERT
	return normals;
}

template <class I>
vector<I> gridIndices(I numVertices, I strideX, bool wrapX, bool wrapY){
	vector<I> r;
	r.reserve(numVertices*4);
	I wx = wrapX ? 0 : strideX;
	for (I i=0; i < numVertices-wx; ++i){
		I o = 1;
		if (i % strideX == strideX-1){
			if (wrapY) o = -(strideX-1);
			else continue;
		}
		r.push_back(i);
		r.push_back((i+strideX)%numVertices);
		r.push_back((i+o+strideX)%numVertices);
		r.push_back(i+o);
	}
	return r;
}

template <class I>
void loadOFF(const char* file, vector<vec3>& vertices, vector<I>& indices){
	cout << "loading: " << file << endl;
	ifstream fin(file);
	if (fin.fail())
		error(string("unable to open off file: ") + file);

	string line;
	int numVertices, numFaces;
	getline(fin, line);
	//cout << line << endl;
	getline(fin, line);
	//cout << line << endl;

	stringstream ss(line);
	ss >> numVertices;
	ss >> numFaces;
	cout << "vertices="<<numVertices << " faces="<< numFaces << endl;

	for (int i=0; i < numVertices; i++){
		getline(fin, line);
		stringstream ss(line);
		vec3 v;
		ss >> v.x;
		ss >> v.y;
		ss >> v.z;

		vertices.push_back(v);
	}

	int faceSize=-1;
	for (int i=0; i < numFaces; i++){
		getline(fin, line);
		stringstream ss(line);
		/*use this to allow faces other than triangle, will fail for files with multiple face sizes
		int tmp;
		ss >> tmp;
		assert(faceSize == -1 || faceSize == tmp);
		faceSize = tmp;

		for (int j=0; j < faceSize; j++){
			I index;
			ss >> index;
			indices.push_back(index);
		}
		*/

		ss >> faceSize;
		vector<I> face;
		for (int j=0; j < faceSize; j++){
			I index;
			ss >> index;
			face.push_back(index);
		}

		//convert face into triangles
		for (U j=1; j < face.size()-1; ++j){
			indices.push_back(face[0]);
			indices.push_back(face[j]);
			indices.push_back(face[j+1]);
		}
	}
}