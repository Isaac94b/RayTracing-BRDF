#ifndef _TRIANGLE_H
#define _TRIANGLE_H


#include "math.h"
#include "Object.h"
#include "Vect.h"
#include "Color.h"
//#include "BoundingBox.hpp"




#define MAX(a, b) ((a) < (b) ? (b) : (a))
#define MAX3(a, b, c) MAX( MAX(a ,b) ,c)
#define MIN(a, b) ((a) > (b) ? (b) : (a))
#define MIN3(a, b, c) MIN( MIN(a, b) ,c)

class Triangle : public Object {

    
    
public :

	Vect A, B, C; 
	Vect normal;
	float distance;
	Color color;
    Color AmbientIntensity;
    Color DiffuseIntensity;
    Color SpecularIntensity;
    
 //   BoundingBox bound;

    
   
    

	Triangle ();

	Triangle(Vect, Vect,Vect, Color);
    
    Triangle(Vect, Vect, Vect, Color,  Color, Color);
    Triangle (Vect PointA , Vect PointB, Vect PointC, Color material, Color AmbientIntensityValue, Color DiffuseIntensityValue, Color SpecularIntensityValue);
    // Object serialization
//    void serialize(std::ostream &out) const;
//    void deserialize(std::istream &in);
    
    
    // Computes bounding box of Triangle and stores result in b
//    void compute_bound();
    
    
//    BoundingBox get_bounding_box();
//    Vect get_midpoint();
    
    

	//method functions
	
	Vect getTriangleNormal() { 

		Vect CA(C.getVectX() - A.getVectX(),
			C.getVectY() - A.getVectY(),
			C.getVectZ() - A.getVectZ());

			Vect BA(B.getVectX() - A.getVectX(),
			B.getVectY() - A.getVectY(),
			B.getVectZ() - A.getVectZ());

		normal = CA.crossProduct(BA).normalize();

		return normal;

	}
    
	float getTriangleDistance() {

		normal = getTriangleNormal();
		distance = normal.dotProduct(A);
		return distance;



	}
    
	virtual Color getColor() { return color;}
    virtual Color getAmbientIntensity() { return AmbientIntensity;}
    virtual Color getDiffuseIntensity() { return DiffuseIntensity;}
    virtual Color getSpecularIntensity() { return SpecularIntensity;}


	virtual Vect getNormalAt(Vect point) {

		normal = getTriangleNormal();

		return normal;

	}

    virtual double findIntersection(Ray ray){

		Vect ray_direction = ray.getRayDirection();
		Vect ray_origin = ray.getRayOrigin();


		normal = getTriangleNormal();
		distance = getTriangleDistance();

		float a = ray_direction.dotProduct(normal);

		if (a==0)
		{
			// ray is parallel to the Triangle
			return -1;
		}
		else{
			
			float b = normal.dotProduct(ray.getRayOrigin().vectAdd(normal.vectMult(distance).negative()));
			float distance2plane = -1*b/a;

			float Qx = ray_direction.vectMult(distance2plane).getVectX() + ray_origin.getVectX();
			float Qy = ray_direction.vectMult(distance2plane).getVectY() + ray_origin.getVectY();
			float Qz = ray_direction.vectMult(distance2plane).getVectZ() + ray_origin.getVectZ();

			Vect Q (Qx, Qy, Qz);
			//we have three conditions and if this three
			//and if this three conditions are true we are inside 
			// the triangle 

			//[CAxQA]*n> = 0 Q point of intersection

			Vect CA(C.getVectX() - A.getVectX(),
			C.getVectY() - A.getVectY(),
			C.getVectZ() - A.getVectZ());

			Vect QA(Q.getVectX() - A.getVectX(),
			Q.getVectY() - A.getVectY(),
			Q.getVectZ() - A.getVectZ());

			float test1 = (CA.crossProduct(QA)).dotProduct(normal)*0.00001;
			
			//[BCxQC]*n> = 0 Q point of intersection
			Vect BC(B.getVectX() - C.getVectX(),
			B.getVectY() - C.getVectY(),
			B.getVectZ() - C.getVectZ());

			Vect QC(Q.getVectX() - C.getVectX(),
			Q.getVectY() - C.getVectY(),
			Q.getVectZ() - C.getVectZ());

			float test2 = (BC.crossProduct(QC)).dotProduct(normal);

			//[ABxQB]*n> = 0 Q point of intersection

			Vect AB(A.getVectX() - B.getVectX(),
			A.getVectY() - B.getVectY(),
			A.getVectZ() - B.getVectZ());

			Vect QB(Q.getVectX() - B.getVectX(),
			Q.getVectY() - B.getVectY(),
			Q.getVectZ() - B.getVectZ());

			float test3 = (AB.crossProduct(QB)).dotProduct(normal);


			if(test1  >=0 && test2 >= 0 && test3>= 0){

			//distance ray origin to the point of intersection

				// inside triangle 
				return -1*b/a*0.0001;

			}
			else{

				//outside the triangle 
				return -1;	
			}

		}




	}
    
   

    
    

    
    
    
};

Triangle::Triangle (){

	
	A= Vect(1,0,0);
	B= Vect(0,1,0);
	C= Vect(0,0,1);
	color = Color(0.5,0.5,0.5, 0);

    AmbientIntensity = Color(0.5,0.5,0.5, 0);
    DiffuseIntensity = Color(0.5,0.5,0.5, 0);
    SpecularIntensity = Color(0.5,0.5,0.5, 0);
    
//    compute_bound();

}


Triangle::Triangle (Vect PointA, Vect PointB, Vect PointC, Color colorValue){

	A = PointA;
	B = PointB;
	C = PointC;

	color = colorValue;
    AmbientIntensity = Color(0.0,0.0,0.0, 0);
    DiffuseIntensity = Color(0.0,0.0,0.0, 0);
    SpecularIntensity = Color(0.0,0.0,0.0, 0);
//    compute_bound();
	
}


Triangle::Triangle (Vect PointA , Vect PointB, Vect PointC, Color AmbientIntensityValue, Color DiffuseIntensityValue, Color SpecularIntensityValue){
    
    A = PointA;
	B = PointB;
	C = PointC;

    //color = Color(1,0.0,0.0, 0);
    AmbientIntensity = AmbientIntensityValue;
    DiffuseIntensity = DiffuseIntensityValue;
    SpecularIntensity = SpecularIntensityValue;
    
//    compute_bound();

}
Triangle::Triangle (Vect PointA , Vect PointB, Vect PointC,Color material, Color AmbientIntensityValue, Color DiffuseIntensityValue, Color SpecularIntensityValue){
    
    A = PointA;
	B = PointB;
	C = PointC;

    color = material;
    AmbientIntensity = AmbientIntensityValue;
    DiffuseIntensity = DiffuseIntensityValue;
    SpecularIntensity = SpecularIntensityValue;
    
//    compute_bound();

}
/*

void Triangle::compute_bound() {
    bound.max[0] = MAX3(A.getVectX(), A.getVectY(), A.getVectZ());
    bound.min[0] = MIN3(A.getVectX(), A.getVectY(), A.getVectZ());
    
    bound.max[1] = MAX3(B.getVectX(), B.getVectY(), B.getVectZ());
    bound.min[1] = MIN3(B.getVectX(), B.getVectY(), B.getVectZ());
    
    bound.max[2] = MAX3(C.getVectX(), C.getVectY(), C.getVectZ());
    bound.min[2] = MIN3(C.getVectX(), C.getVectY(), C.getVectZ());
}

void Triangle::serialize(std::ostream &out) const {
    bound.serialize(out);
}

void Triangle::deserialize(std::istream &in) {
    bound.deserialize(in);
}

BoundingBox Triangle::get_bounding_box(){
    
    return bound;
    
}

Vect Triangle :: get_midpoint()
{
    
    //need to be implemented
    
    return Vect(0,0,0);
}
*/


#endif