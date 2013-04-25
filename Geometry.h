#pragma once

typedef vec4 Plane;


//I'm sure their has to be an easier way but this is what all the tutorials show
struct Frustum {
	Plane l,r,t,b,n,f;
	
	//corners - nbl,nbr,ntr,ntl,fbl,fbr,ftr,ftl
	vec3 corners[8];
	vec3 center;

	Frustum(mat4 eye){
		auto i = inverse(eye);
		
		//this method seems to have a lot of numerical error
		vec4 w;
		w = i * vec4(-1,-1,-1,1); corners[0] = vec3(w/w.w);
		w = i * vec4(1,-1,-1,1); corners[1] = vec3(w/w.w);
		w = i * vec4(1,1,-1,1); corners[2] = vec3(w/w.w);
		w = i * vec4(-1,1,-1,1); corners[3] = vec3(w/w.w);
		w = i * vec4(-1,-1,1,1); corners[4] = vec3(w/w.w);
		w = i * vec4(1,-1,1,1); corners[5] = vec3(w/w.w);
		w = i * vec4(1,1,1,1); corners[6] = vec3(w/w.w);
		w = i * vec4(-1,1,1,1); corners[7] = vec3(w/w.w);

		center = vec3(0);
		for (int i=0; i < 8; i++)
			center += corners[i];
		center /= 8;

		eye = transpose(eye);
		l = eye[0] + eye[3];
		r = -eye[0] + eye[3];
		b = eye[1] + eye[3];
		t = -eye[1] + eye[3];
		n = eye[2] + eye[3];
		f = -eye[2] + eye[3];

		//normalize planes
		l /= length(vec3(l));
		r /= length(vec3(r));
		b /= length(vec3(b));
		t /= length(vec3(t));
		n /= length(vec3(n));
		f /= length(vec3(f));
	}
};

struct Intersect {
	//info about an intersection
	struct Result {
		bool intersect;
		vec3 pt;
	};
};

struct IBoundingVolume {
	vec3 center;
	bool sleeping;
	IBoundingVolume():sleeping(false){}
	virtual Intersect::Result intersect(IBoundingVolume* bv)=0;
	virtual Intersect::Result intersect(BoundingSphere* bs)=0;
	virtual Intersect::Result intersect(BoundingAABB* ba)=0;
	virtual bool inside(Frustum& f)=0;
	virtual bool intersect(Plane p)=0;
	virtual void transform(mat4 m, IBoundingVolume*& result)=0;//allocates new BV if result == NULL
	virtual ~IBoundingVolume(){}

	/*
	Intersect::Result intersect(IBoundingVolume* bv){
		//do with polymorphism eventually
		auto s1 = dynamic_cast<Sphere*>(this);
		auto s2 = dynamic_cast<Sphere*>(bv);
		if (s1 && s2)
			return Intersect::sphere_sphere(*s1, *s2);

		assert(false && "Unsupported bounding volume intersection");
		return Intersect::Result();
	}
	*/
};

struct BoundingAABB : public IBoundingVolume {
	vec3 min, max;
	BoundingAABB(vec3* vertices, size_t size){
		min = vec3(FLT_MAX);
		max = vec3(-FLT_MAX);
		for (size_t i=0; i < size; i++){
			auto& v = vertices[i];
			if (v.x < min.x) min.x = v.x;
			if (v.x > max.x) max.x = v.x;
			if (v.y < min.y) min.y = v.y;
			if (v.y > max.y) max.y = v.y;
			if (v.z < min.z) min.z = v.z;
			if (v.z > max.z) max.z = v.z;
		}
		center = (min + max) / 2.f;
	}

	Intersect::Result intersect(IBoundingVolume* bv){
		return bv->intersect(this);
	}

	//not implemented
	Intersect::Result intersect(BoundingSphere* bs){
		assert(false && "Unsupported bounding volume intersection");
		return Intersect::Result();
	}

	//not implemented
	Intersect::Result intersect(BoundingAABB* ba){
		assert(false && "Unsupported bounding volume intersection");
		return Intersect::Result();
	}

	//not implemented
	virtual bool inside(Frustum& f){
		return true;
	}

	//not implemented
	virtual bool intersect(Plane p){
		return true;
	}

	virtual void transform(mat4 m, IBoundingVolume*& result){
		
	}
	virtual ~BoundingAABB(){}

};


struct BoundingSphere : public IBoundingVolume {
	float radius;

	BoundingSphere(){}
	BoundingSphere(vec3* vertices, size_t size){
		BoundingAABB a(vertices,size);
		center = a.center;

		float max_r2 = FLT_MIN;
		for (size_t i=0; i < size; i++){
			auto v = vertices[i] - center;
			float r2 = dot(v,v);
			if (r2 > max_r2) max_r2 = r2; 
		}
		radius = sqrt(max_r2);
	}

	BoundingSphere(vec3 center, float radius){
		this->center = center;
		this->radius = radius;
	}

	Intersect::Result intersect(IBoundingVolume* bv){
		return bv->intersect(this);
	}

	Intersect::Result intersect(BoundingSphere* bs){
		auto& a = *this, b = *bs;
		vec3 d = b.center - a.center;
		float dist2 = dot(d,d);
		float rsum = a.radius + b.radius;

		Intersect::Result r;
		if (dist2 <= rsum * rsum){
			r.intersect = true;
			r.pt = a.center + d * a.radius / (a.radius + b.radius);//somewhere between the overlap
		} else 
			r.intersect = false;
		return r;
	}

	//not implemented
	Intersect::Result intersect(BoundingAABB* ba){
		assert(false && "Unsupported bounding volume intersection");
		return Intersect::Result();
	}

	virtual bool inside(Frustum& f){
		if (dot(center, vec3(f.l)) + f.l.w + radius < 0) return false;
		if (dot(center, vec3(f.r)) + f.r.w + radius < 0) return false;
		if (dot(center, vec3(f.b)) + f.b.w + radius < 0) return false;
		if (dot(center, vec3(f.t)) + f.t.w + radius < 0) return false;
		if (dot(center, vec3(f.n)) + f.n.w + radius < 0) return false;
		if (dot(center, vec3(f.f)) + f.f.w + radius < 0) return false;

		return true;
	}

	virtual bool intersect(Plane p){
		float dist = dot(center, vec3(p)) - p.w;
		return abs(dist) <= radius;
	}

	//used to transform a local space bounding volume into a world space one
	void transform(mat4 m, IBoundingVolume*& result){
		if (!result)
			result = new BoundingSphere();
		auto r = dynamic_cast<BoundingSphere*>(result);
		assert(r && "result must be bounding sphere or NULL");
		r->center = vec3(m * vec4(center,1));
		r->radius = radius * std::max(std::max(m[0][0],m[1][1]),m[2][2]);//not sure if always correct
	}

	virtual ~BoundingSphere(){}
};