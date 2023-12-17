#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>
#include <limits>

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

#include "Vect.h"
#include "Ray.h"
#include "Camera.h"
#include "Color.h"
#include "Source.h"
#include "Light.h"
#include "Object.h"
#include "Sphere.h"
#include "Plane.h"
#include "Triangle.h"

#include "glm\ext.hpp"
#include "glm\glm.hpp"
#include "glm\core\func_geometric.hpp"

#include "obj_loader.h"
//#include "ReyTracing\objLoader.h"

#include <omp.h>

#include "ColorChange.h"
using namespace std;


vector<Triangle> scene_Triangles;

struct RGBType {
	double r;
	double g;
	double b;
};

bool loadOBJ(
	const char * path, 
	std::vector<glm::vec3> & out_vertices, 
	std::vector<glm::vec2> & out_uvs,
	std::vector<glm::vec3> & out_normals
){
	printf("Loading OBJ file %s...\n", path);

	std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
	std::vector<glm::vec3> temp_vertices; 
	std::vector<glm::vec2> temp_uvs;
	std::vector<glm::vec3> temp_normals;


	FILE * file = fopen(path, "r");
	if( file == NULL ){
		printf("Impossible to open the file ! Are you in the right path ? See Tutorial 1 for details\n");
		return false;
	}

	while( 1 ){

		char lineHeader[128];
		// read the first word of the line
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
			break; // EOF = End Of File. Quit the loop.

		// else : parse lineHeader
		
		if ( strcmp( lineHeader, "v" ) == 0 ){
			glm::vec3 vertex;
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z );
			temp_vertices.push_back(vertex);
		}else if ( strcmp( lineHeader, "vt" ) == 0 ){
			glm::vec2 uv;
			fscanf(file, "%f %f\n", &uv.x, &uv.y );
			uv.y = -uv.y; // Invert V coordinate since we will only use DDS texture, which are inverted. Remove if you want to use TGA or BMP loaders.
			temp_uvs.push_back(uv);
		}else if ( strcmp( lineHeader, "vn" ) == 0 ){
			glm::vec3 normal;
			fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z );
			temp_normals.push_back(normal);
		}else if ( strcmp( lineHeader, "f" ) == 0 ){
			std::string vertex1, vertex2, vertex3;
			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
			int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2] );
			if (matches != 9){
				printf("File can't be read by our simple parser :-( Try exporting with other options\n");
				return false;
			}
			vertexIndices.push_back(vertexIndex[0]);
			vertexIndices.push_back(vertexIndex[1]);
			vertexIndices.push_back(vertexIndex[2]);
			uvIndices    .push_back(uvIndex[0]);
			uvIndices    .push_back(uvIndex[1]);
			uvIndices    .push_back(uvIndex[2]);
			normalIndices.push_back(normalIndex[0]);
			normalIndices.push_back(normalIndex[1]);
			normalIndices.push_back(normalIndex[2]);
		}else{
			// Probably a comment, eat up the rest of the line
			char stupidBuffer[1000];
			fgets(stupidBuffer, 1000, file);
		}

	}

	// For each vertex of each triangle
	for( unsigned int i=0; i<vertexIndices.size(); i++ ){

		// Get the indices of its attributes
		unsigned int vertexIndex = vertexIndices[i];
		unsigned int uvIndex = uvIndices[i];
		unsigned int normalIndex = normalIndices[i];
		
		// Get the attributes thanks to the index
		glm::vec3 vertex = temp_vertices[ vertexIndex-1 ];
		glm::vec2 uv = temp_uvs[ uvIndex-1 ];
		glm::vec3 normal = temp_normals[ normalIndex-1 ];
		
		// Put the attributes in buffers
		out_vertices.push_back(vertex);
		out_uvs     .push_back(uv);
		out_normals .push_back(normal);
	
	}

	return true;
}



vector<Triangle> Implementing_Triangles(vector<glm :: vec3> out_vertices,
                                        Color AmbiantMaterial, Color DiffuseMaterial, Color SpecularMaterial){

											
    vector<Triangle> Exported_Triangles;
  
    for (int i=0; i<out_vertices.size()/3 ; i++ ) {
		
        Triangle scene_Triangle (

                                 Vect(out_vertices[i+i+i+1].x,out_vertices[i+i+i+1].y,out_vertices[i+i+i+1].z)  ,
                                 Vect (out_vertices[i+i+i].x,out_vertices[i+i+i].y,out_vertices[i+i+i].z) ,
								 Vect(out_vertices[i+i+i+2].x,out_vertices[i+i+i+2].y,out_vertices[i+i+i+2].z),AmbiantMaterial, DiffuseMaterial, SpecularMaterial );


		
       /*  cout << "point A  :" << out_vertices[i+i+i].x <<"|" << out_vertices[i+i+i].y <<"|" << out_vertices[i+i+i].z<<"\n"<<endl;
         cout << "point B  :" << out_vertices[i+i+i+1].x <<"|" << out_vertices[i+i+i+1].y <<"|" << out_vertices[i+i+i+1].z<<"\n"<<endl;
         cout << "point C  :"  << out_vertices[i+i+i+2].x <<"|" << out_vertices[i+i+i+2].y<<"|"  << out_vertices[i+i+i+2].z<<"\n"<<endl;
		 */
        cout <<"\n";
		
        Exported_Triangles.push_back(scene_Triangle);


    }
    return Exported_Triangles;
}

void savebmp (const char *filename, int w, int h, int dpi, RGBType *data) {
	FILE *f;
	int k = w*h;
	int s = 4*k;
	int filesize = 54 + s;
	
	double factor = 39.375;
	int m = static_cast<int>(factor);
	
	int ppm = dpi*m;
	
	unsigned char bmpfileheader[14] = {'B','M', 0,0,0,0, 0,0,0,0, 54,0,0,0};
	unsigned char bmpinfoheader[40] = {40,0,0,0, 0,0,0,0, 0,0,0,0, 1,0,24,0};
	
	bmpfileheader[ 2] = (unsigned char)(filesize);
	bmpfileheader[ 3] = (unsigned char)(filesize>>8);
	bmpfileheader[ 4] = (unsigned char)(filesize>>16);
	bmpfileheader[ 5] = (unsigned char)(filesize>>24);
	
	bmpinfoheader[ 4] = (unsigned char)(w);
	bmpinfoheader[ 5] = (unsigned char)(w>>8);
	bmpinfoheader[ 6] = (unsigned char)(w>>16);
	bmpinfoheader[ 7] = (unsigned char)(w>>24);
	
	bmpinfoheader[ 8] = (unsigned char)(h);
	bmpinfoheader[ 9] = (unsigned char)(h>>8);
	bmpinfoheader[10] = (unsigned char)(h>>16);
	bmpinfoheader[11] = (unsigned char)(h>>24);
	
	bmpinfoheader[21] = (unsigned char)(s);
	bmpinfoheader[22] = (unsigned char)(s>>8);
	bmpinfoheader[23] = (unsigned char)(s>>16);
	bmpinfoheader[24] = (unsigned char)(s>>24);
	
	bmpinfoheader[25] = (unsigned char)(ppm);
	bmpinfoheader[26] = (unsigned char)(ppm>>8);
	bmpinfoheader[27] = (unsigned char)(ppm>>16);
	bmpinfoheader[28] = (unsigned char)(ppm>>24);
	
	bmpinfoheader[29] = (unsigned char)(ppm);
	bmpinfoheader[30] = (unsigned char)(ppm>>8);
	bmpinfoheader[31] = (unsigned char)(ppm>>16);
	bmpinfoheader[32] = (unsigned char)(ppm>>24);
	
	f = fopen(filename,"wb");
	
	fwrite(bmpfileheader,1,14,f);
	fwrite(bmpinfoheader,1,40,f);
	
	for (int i = 0; i < k; i++) {
		RGBType rgb = data[i];
		
		double red = (data[i].r)*255;
		double green = (data[i].g)*255;
		double blue = (data[i].b)*255;
		
		unsigned char color[3] = {(int)floor(blue),(int)floor(green),(int)floor(red)};
		
		fwrite(color,1,3,f);
	}
	
	fclose(f);
}

int winningObjectIndex(vector<double> object_intersections) {
	// return the index of the winning intersection
	int index_of_minimum_value;
	
	// prevent unnessary calculations
	if (object_intersections.size() == 0) {
		// if there are no intersections
		return -1;
	}
	else if (object_intersections.size() == 1) {
		if (object_intersections.at(0) > 0) {
			// if that intersection is greater than zero then its our index of minimum value
			return 0;
		}
		else {
			// otherwise the only intersection value is negative
			return -1;
		}
	}
	else {
		// otherwise there is more than one intersection
		// first find the maximum value
		
		double max = 0;
		for (int i = 0; i < object_intersections.size(); i++) {
			if (max < object_intersections.at(i)) {
				max = object_intersections.at(i);
			}
		}
		
		// then starting from the maximum value find the minimum positive value
		if (max > 0) {
			// we only want positive intersections
			for (int index = 0; index < object_intersections.size(); index++) {
				if (object_intersections.at(index) > 0 && object_intersections.at(index) <= max) {
					max = object_intersections.at(index);
					index_of_minimum_value = index;
				}
			}
			
			return index_of_minimum_value;
		}
		else {
			// all the intersections were negative
			return -1;
		}
	}
}

Color getColorAt(Vect intersection_position, Vect intersecting_ray_direction, vector<Object*> scene_objects, int index_of_winning_object, vector<Source*> light_sources, double accuracy, double ambientlight) {
	
	Color winning_object_color =  scene_objects.at(index_of_winning_object)->getColor(); // hada l material 
	Vect winning_object_normal = scene_objects.at(index_of_winning_object)->getNormalAt(intersection_position);
	
	if (winning_object_color.getColorSpecial() == 2) {
		// checkered/tile floor pattern
		// sama
		int square = (int)floor(intersection_position.getVectX()) + (int)floor(intersection_position.getVectZ());
		
		if ((square % 2) == 0) {
			// black tile
			winning_object_color.setColorRed(0);
			winning_object_color.setColorGreen(0);
			winning_object_color.setColorBlue(0);
		}
		else {
			// white tile
			winning_object_color.setColorRed(1);
			winning_object_color.setColorGreen(1);
			winning_object_color.setColorRed(1);
		}
	}
	
	Color final_color = winning_object_color.colorScalar(ambientlight);
	
	if (winning_object_color.getColorSpecial() > 0 && winning_object_color.getColorSpecial() <= 1) {
		// reflection from objects with specular intensity
		double dot1 = winning_object_normal.dotProduct(intersecting_ray_direction.negative());
		Vect scalar1 = winning_object_normal.vectMult(dot1);
		Vect add1 = scalar1.vectAdd(intersecting_ray_direction);
		Vect scalar2 = add1.vectMult(2);
		Vect add2 = intersecting_ray_direction.negative().vectAdd(scalar2);
		Vect reflection_direction = add2.normalize();
		
		Ray reflection_ray (intersection_position, reflection_direction);
		
		// determine what the ray intersects with first
		vector<double> reflection_intersections;
		
		for (int reflection_index = 0; reflection_index < scene_objects.size(); reflection_index++) {
			reflection_intersections.push_back(scene_objects.at(reflection_index)->findIntersection(reflection_ray));
		}
		
		int index_of_winning_object_with_reflection = winningObjectIndex(reflection_intersections);
		
		if (index_of_winning_object_with_reflection != -1) {
			// reflection ray missed everthing else
			if (reflection_intersections.at(index_of_winning_object_with_reflection) > accuracy) {
				// determine the position and direction at the point of intersection with the reflection ray
				// the ray only affects the color if it reflected off something
				
				Vect reflection_intersection_position = intersection_position.vectAdd(reflection_direction.vectMult(reflection_intersections.at(index_of_winning_object_with_reflection)));
				Vect reflection_intersection_ray_direction = reflection_direction;
				
				Color reflection_intersection_color = getColorAt(reflection_intersection_position, reflection_intersection_ray_direction, scene_objects, index_of_winning_object_with_reflection, light_sources, accuracy, ambientlight);

				//Color tmprcolor = final_color.colorAdd(reflection_intersection_color.colorScalar(winning_object_color.getColorSpecial()));
				//final_color = HSV_Type(tmprcolor);
				//final_color = XYZ_Type(tmprcolor);
				//final_color = YCbCr_Type(tmprcolor);
				//final_color = YIQ_Type(tmprcolor);
				//final_color = YUV_Type(tmprcolor);
				final_color = final_color.colorAdd(reflection_intersection_color.colorScalar(winning_object_color.getColorSpecial()));
			}
		}
	} 
	
	for (int light_index = 0; light_index < light_sources.size(); light_index++) {
		Vect light_direction = light_sources.at(light_index)->getLightPosition().vectAdd(intersection_position.negative()).normalize();
		
		float cosine_angle = winning_object_normal.dotProduct(light_direction);
		
		if (cosine_angle > 0) {
			// test for shadows
			bool shadowed = false;
			
			Vect distance_to_light = light_sources.at(light_index)->getLightPosition().vectAdd(intersection_position.negative()).normalize();
			float distance_to_light_magnitude = distance_to_light.magnitude();
			
			Ray shadow_ray (intersection_position, light_sources.at(light_index)->getLightPosition().vectAdd(intersection_position.negative()).normalize());
			
			vector<double> secondary_intersections;
			
			for (int object_index = 0; object_index < scene_objects.size() && shadowed == false; object_index++) {
				secondary_intersections.push_back(scene_objects.at(object_index)->findIntersection(shadow_ray));
			}
			
			for (int c = 0; c < secondary_intersections.size(); c++) {
				if (secondary_intersections.at(c) > accuracy) {
					if (secondary_intersections.at(c) <= distance_to_light_magnitude) {
						shadowed = true;
					}
				}
				break;
			}
			
			if (shadowed == false) {

				if (light_sources.at(light_index)->getLightColor().getColorRed()!=0 &&
                light_sources.at(light_index)->getLightColor().getColorGreen()!=0 &&
                light_sources.at(light_index)->getLightColor().getColorBlue()!=0 ) {

				//Color tmprcolor = final_color.colorAdd(winning_object_color.colorMultiply(light_sources.at(light_index)->getLightColor()).colorScalar(cosine_angle));
				//final_color = HSV_Type(tmprcolor);
				//final_color = XYZ_Type(tmprcolor);
				//final_color = YCbCr_Type(tmprcolor);
				//final_color = YIQ_Type(tmprcolor);
				//final_color = YUV_Type(tmprcolor);
				final_color = final_color.colorAdd(winning_object_color.colorMultiply(light_sources.at(light_index)->getLightColor()).colorScalar(cosine_angle));


				
				if (winning_object_color.getColorSpecial() > 0 && winning_object_color.getColorSpecial() <= 1) {
					// special [0-1]
					double dot1 = winning_object_normal.dotProduct(intersecting_ray_direction.negative());
					Vect scalar1 = winning_object_normal.vectMult(dot1);
					Vect add1 = scalar1.vectAdd(intersecting_ray_direction);
					Vect scalar2 = add1.vectMult(2);
					Vect add2 = intersecting_ray_direction.negative().vectAdd(scalar2);
					Vect reflection_direction = add2.normalize();
					
					double specular = reflection_direction.dotProduct(light_direction);
					if (specular > 0) {
						specular = pow(specular, 10);
						//Color tmprcolor = final_color.colorAdd(light_sources.at(light_index)->getLightColor().colorScalar(specular*winning_object_color.getColorSpecial()));
						//final_color = HSV_Type(tmprcolor);
						//final_color = XYZ_Type(tmprcolor);
						//final_color = YCbCr_Type(tmprcolor);
						//final_color = YIQ_Type(tmprcolor);
						//final_color = YUV_Type(tmprcolor);
						final_color = final_color.colorAdd(light_sources.at(light_index)->getLightColor().colorScalar(specular*winning_object_color.getColorSpecial()));
					}
				}
				}
				else{
				Color Light_Ambient_Intensity = light_sources.at(light_index)->getLight_Ambient_Intensity();
                Color Light_Diffuse_Intensity = light_sources.at(light_index)->getLight_Diffuse_Intensity();
                Color Light_Specular_Intensity = light_sources.at(light_index)->getLight_Specular_Intensity();
							

                Color winning_object_Ambient_Intensity  = scene_objects.at(index_of_winning_object)->getAmbientIntensity();

                Color winning_object_Diffuse_Intensity  = scene_objects.at(index_of_winning_object)->getDiffuseIntensity();

                Color winning_object_Specular_Intensity = scene_objects.at(index_of_winning_object)->getSpecularIntensity();
				
				//LAMPERT MODEL
				if(winning_object_Ambient_Intensity.getColorSpecial() == 3){
						
					
					final_color = final_color.colorAdd(Light_Diffuse_Intensity.colorMultiply(winning_object_Diffuse_Intensity).colorScalar(light_direction.dotProduct(winning_object_normal)));

				}

				//PHONG MODEL

				if(winning_object_Ambient_Intensity.getColorSpecial() ==4){
					double LMultNormal = 2 * (light_direction.dotProduct(winning_object_normal));
                    Vect New = winning_object_normal.vectMult(LMultNormal);
                    Vect RD = light_direction.vectSous(New);

                    // I = Ia*Ka + Id*Kd(N . L) + Is*Ks(R . V)coeff
                    final_color = final_color.colorAdd((Light_Ambient_Intensity.colorMultiply(winning_object_Ambient_Intensity)).colorAdd( (Light_Diffuse_Intensity.colorMultiply(winning_object_Diffuse_Intensity)).colorScalar(light_direction.dotProduct(winning_object_normal)).colorAdd((Light_Specular_Intensity.colorMultiply(winning_object_Specular_Intensity))).colorScalar(pow(RD.dotProduct(intersecting_ray_direction.normalize()) , 9))));
/*
                    // reflection from objects with specular intensity
                    double dot1 = winning_object_normal.dotProduct(intersecting_ray_direction.negative());
                    Vect scalar1 = winning_object_normal.vectMult(dot1);
                    Vect add1 = scalar1.vectAdd(intersecting_ray_direction);
                    Vect scalar2 = add1.vectMult(2);
                    Vect add2 = intersecting_ray_direction.negative().vectAdd(scalar2);
                    Vect reflection_direction = add2.normalize();


                    Ray reflection_ray(intersection_position, reflection_direction);

                    //determine what the ray intersects with first
                    vector<double> reflection_intersections;

                    for (int reflection_index = 0; reflection_index < scene_objects.size(); reflection_index++){

                        reflection_intersections.push_back(scene_objects.at(reflection_index)->findIntersection(reflection_ray));

                    }

                    int index_of_winning_object_with_reflection = winningObjectIndex(reflection_intersections);

                    if(index_of_winning_object_with_reflection != -1){

                        //reflection ray missed everything else

                        if(reflection_intersections.at(index_of_winning_object_with_reflection) > accuracy){

                            // determine the position and direction at the point of intersection with the reflection
                            //the ray only affects the color if it reflected off something

                            Vect reflection_intersection_position = intersection_position.vectAdd(reflection_direction.vectMult(reflection_intersections.at(index_of_winning_object_with_reflection)));
                            Vect reflection_intersecting_ray_direction = reflection_direction;


                            Color reflection_intersection_color = getColorAt(reflection_intersection_position, reflection_intersecting_ray_direction, scene_objects, index_of_winning_object_with_reflection, light_sources, accuracy, ambientlight);

                            final_color = final_color.colorAdd(reflection_intersection_color.colorScalar(0.3));

                        }
                    }
					*/
				}

				//WARD ISOTROPIC MODEL
				if(winning_object_Ambient_Intensity.getColorSpecial() == 5){
					Vect  LplusVision = light_direction.vectAdd(intersecting_ray_direction.negative().normalize());
                    float Lmagnitude = light_direction.magnitude();
                    float Vmagnitude = intersecting_ray_direction.negative().normalize().magnitude();
                    Vect H = LplusVision.vectMult( (1/(Lmagnitude+Vmagnitude)));
				    float Cosine_Incoming_Angle = light_direction.dotProduct(winning_object_normal);
                    float Cosine_Outgoing_Angle = winning_object_normal.dotProduct(intersecting_ray_direction.negative().normalize());
                    float Alpha = acos(winning_object_normal.dotProduct(H));
                    float Surface_Roughness=0.45;
                    float PI = 3.14;
                    float CosineInMultiCosineOut = Cosine_Incoming_Angle * Cosine_Outgoing_Angle;								
                    float Kspec = ((1)/(sqrtf(CosineInMultiCosineOut)))*((exp(-1*((pow(tan(Alpha),2))/(pow(Surface_Roughness,2)))))/(4*PI*pow(Surface_Roughness,2)));
					
                    final_color = winning_object_Ambient_Intensity.colorMultiply(Light_Ambient_Intensity).colorAdd(winning_object_Diffuse_Intensity.colorMultiply(Light_Diffuse_Intensity).colorScalar(winning_object_normal.dotProduct(light_direction))).colorAdd(
                                                                                                                                                                                                  winning_object_Specular_Intensity.colorMultiply(Light_Specular_Intensity).colorScalar(Kspec));


					/*
                    // reflection from objects with specular intensity
                    double dot1 = winning_object_normal.dotProduct(intersecting_ray_direction.negative());
                    Vect scalar1 = winning_object_normal.vectMult(dot1);
                    Vect add1 = scalar1.vectAdd(intersecting_ray_direction);
                    Vect scalar2 = add1.vectMult(2);
                    Vect add2 = intersecting_ray_direction.negative().vectAdd(scalar2);
                    Vect reflection_direction = add2.normalize();


                    Ray reflection_ray(intersection_position, reflection_direction);

                    //determine what the ray intersects with first
                    vector<double> reflection_intersections;

                    for (int reflection_index = 0; reflection_index < scene_objects.size(); reflection_index++){

                        reflection_intersections.push_back(scene_objects.at(reflection_index)->findIntersection(reflection_ray));

                    }

                    int index_of_winning_object_with_reflection = winningObjectIndex(reflection_intersections);

                    if(index_of_winning_object_with_reflection != -1){

                        //reflection ray missed everything else

                        if(reflection_intersections.at(index_of_winning_object_with_reflection) > accuracy){

                            // determine the position and direction at the point of intersection with the reflection
                            //the ray only affects the color if it reflected off something

                            Vect reflection_intersection_position = intersection_position.vectAdd(reflection_direction.vectMult(reflection_intersections.at(index_of_winning_object_with_reflection)));
                            Vect reflection_intersecting_ray_direction = reflection_direction;


                            Color reflection_intersection_color = getColorAt(reflection_intersection_position, reflection_intersecting_ray_direction, scene_objects, index_of_winning_object_with_reflection, light_sources, accuracy, ambientlight);

                            final_color = final_color.colorAdd(reflection_intersection_color.colorScalar(0.5));

                        }

                    }
				*/	
				}
				

				//WARD ANISTROPIC MODEL
				if(winning_object_Ambient_Intensity.getColorSpecial() >=6 && winning_object_Ambient_Intensity.getColorSpecial() <7 ){
					 double LMultNormal = 2 * (light_direction.dotProduct(winning_object_normal));
                    Vect New = winning_object_normal.vectMult(LMultNormal);
                    Vect RD = light_direction.vectSous(New);
                    Vect  LplusVision = light_direction.vectAdd(intersecting_ray_direction.negative().normalize());
                    float Lmagnitude = light_direction.magnitude();
                    float Vmagnitude = intersecting_ray_direction.negative().normalize().magnitude();
                    Vect H = LplusVision.vectMult( (1/(Lmagnitude+Vmagnitude)));
                    Vect Direction = Vect(0.0,1.0, 0.0);
                    if ( winning_object_Ambient_Intensity.getColorSpecial() ==6.7){
                        Direction = Vect(0.0,0.0, 1.0);
                    }					
					Direction = Direction.normalize();
                    Vect x = winning_object_normal.crossProduct(Direction);
                    x = x.normalize();
                    Vect y = winning_object_normal.crossProduct(x);
                    y = y.normalize();
                    float Cosine_Incoming_Angle = light_direction.dotProduct(winning_object_normal);
                    float Cosine_Outgoing_Angle = winning_object_normal.dotProduct(intersecting_ray_direction.negative().normalize());
                    float Alpha = acos(winning_object_normal.dotProduct(H));
                    double Surface_Roughnessx;
                    double Surface_Roughnessy;				
					 if ( winning_object_Ambient_Intensity.getColorSpecial() ==6.5)
                    {
                        Surface_Roughnessx=0.9;
                        Surface_Roughnessy=0.5;
                    }

                    if ( winning_object_Ambient_Intensity.getColorSpecial() ==6.7)
                    {

                        Surface_Roughnessx=0.2;
                        Surface_Roughnessy=0.7  ;
                    }
					float PI = 3.14;
                    float CosineInMultiCosineOut = Cosine_Incoming_Angle * Cosine_Outgoing_Angle;
                    double FirstTerm = (1)/(sqrtf(CosineInMultiCosineOut));
                   double SecondTerm =  (1/PI*Surface_Roughnessx*Surface_Roughnessy);
                    double ThirdTerm = exp(-2*((pow((H.dotProduct(x)/Surface_Roughnessx),2)+pow((H.dotProduct(y)/Surface_Roughnessy),2))/(1+ H.dotProduct(winning_object_normal))));
                    double Kspec =  FirstTerm * ThirdTerm;	
					        if ( winning_object_Ambient_Intensity.getColorSpecial() ==6.5){
                        final_color = winning_object_Ambient_Intensity.colorMultiply(Light_Ambient_Intensity).colorAdd(winning_object_Diffuse_Intensity.colorMultiply(Light_Diffuse_Intensity).colorScalar(winning_object_normal.dotProduct(light_direction))).colorAdd(
                                                                                                                                                                                                                                                      winning_object_Specular_Intensity.colorMultiply(Light_Specular_Intensity).colorScalar(pow(Kspec,76.8)));

                    }
                    if ( winning_object_Ambient_Intensity.getColorSpecial() ==6.7){
                        final_color = winning_object_Ambient_Intensity.colorMultiply(Light_Ambient_Intensity).colorAdd(winning_object_Diffuse_Intensity.colorMultiply(Light_Diffuse_Intensity).colorScalar(winning_object_normal.dotProduct(light_direction)).colorScalar(1/PI)).colorAdd(                                                                                                                                                                                                                                                                    winning_object_Specular_Intensity.colorMultiply(Light_Specular_Intensity).colorScalar(Kspec));
                    }
					/*
					double dot1 = winning_object_normal.dotProduct(intersecting_ray_direction.negative());
                    Vect scalar1 = winning_object_normal.vectMult(dot1);
                    Vect add1 = scalar1.vectAdd(intersecting_ray_direction);
                    Vect scalar2 = add1.vectMult(2);
                    Vect add2 = intersecting_ray_direction.negative().vectAdd(scalar2);
                    Vect reflection_direction = add2.normalize();


                    Ray reflection_ray(intersection_position, reflection_direction);

                    //determine what the ray intersects with first
                    vector<double> reflection_intersections;

                    for (int reflection_index = 0; reflection_index < scene_objects.size(); reflection_index++){

                        reflection_intersections.push_back(scene_objects.at(reflection_index)->findIntersection(reflection_ray));

                    }
					

					int index_of_winning_object_with_reflection = winningObjectIndex(reflection_intersections);

                    if(index_of_winning_object_with_reflection != -1){

                        //reflection ray missed everything else

                        if(reflection_intersections.at(index_of_winning_object_with_reflection) > accuracy){

                            // determine the position and direction at the point of intersection with the reflection
                            //the ray only affects the color if it reflected off something

                            Vect reflection_intersection_position = intersection_position.vectAdd(reflection_direction.vectMult(reflection_intersections.at(index_of_winning_object_with_reflection)));
                            Vect reflection_intersecting_ray_direction = reflection_direction;


                            Color reflection_intersection_color = getColorAt(reflection_intersection_position, reflection_intersecting_ray_direction, scene_objects, index_of_winning_object_with_reflection, light_sources, accuracy, ambientlight);

                            if (winning_object_Ambient_Intensity.getColorSpecial() ==13.5)
                            {
                                final_color = final_color.colorAdd(reflection_intersection_color.colorScalar(0.8));
                            }



                            if (winning_object_Ambient_Intensity.getColorSpecial() ==13.7)
                            {
                                final_color = final_color.colorAdd(reflection_intersection_color.colorScalar(0.1));
                            }


                        }
                    }

					*/
				}

				//Gaussian MODEL
				if ( winning_object_Ambient_Intensity.getColorSpecial() == 8)
                {



                    Vect  LplusVision = light_direction.vectAdd(intersecting_ray_direction.negative().normalize());
                    double Lmagnitude = light_direction.magnitude();
                    double Vmagnitude = intersecting_ray_direction.negative().normalize().magnitude();
                    Vect H = LplusVision.vectMult( (1/(Lmagnitude+Vmagnitude)));
                    double m=1;


                    float cosine_Angle =H.dotProduct(winning_object_normal);//.dotProduct(H);


                    float Angle = acos(cosine_Angle);
                    double Auto_Correlation= 3;

                    //Ks = exp(-(Angle entre  N,H)/m)2
                    double Kspec =Auto_Correlation * exp((-1) * pow(Angle/m, 2));

                    final_color = (Light_Ambient_Intensity.colorMultiply(winning_object_Ambient_Intensity)).colorAdd( (Light_Diffuse_Intensity.colorMultiply(winning_object_Diffuse_Intensity)).colorScalar(light_direction.dotProduct(winning_object_normal)).colorAdd((Light_Specular_Intensity.colorMultiply(winning_object_Specular_Intensity))).colorScalar(Kspec));
				/*	
                    // reflection from objects with specular intensity
                    double dot1 = winning_object_normal.dotProduct(intersecting_ray_direction.negative());
                    Vect scalar1 = winning_object_normal.vectMult(dot1);
                    Vect add1 = scalar1.vectAdd(intersecting_ray_direction);
                    Vect scalar2 = add1.vectMult(2);
                    Vect add2 = intersecting_ray_direction.negative().vectAdd(scalar2);
                    Vect reflection_direction = add2.normalize();


                    Ray reflection_ray(intersection_position, reflection_direction);

                    //determine what the ray intersects with first
                    vector<double> reflection_intersections;

                    for (int reflection_index = 0; reflection_index < scene_objects.size(); reflection_index++){

                        reflection_intersections.push_back(scene_objects.at(reflection_index)->findIntersection(reflection_ray));

                    }

                    int index_of_winning_object_with_reflection = winningObjectIndex(reflection_intersections);

                    if(index_of_winning_object_with_reflection != -1){

                        //reflection ray missed everything else

                        if(reflection_intersections.at(index_of_winning_object_with_reflection) > accuracy){

                            // determine the position and direction at the point of intersection with the reflection
                            //the ray only affects the color if it reflected off something

                            Vect reflection_intersection_position = intersection_position.vectAdd(reflection_direction.vectMult(reflection_intersections.at(index_of_winning_object_with_reflection)));
                            Vect reflection_intersecting_ray_direction = reflection_direction;


                            Color reflection_intersection_color = getColorAt(reflection_intersection_position, reflection_intersecting_ray_direction, scene_objects, index_of_winning_object_with_reflection, light_sources, accuracy, ambientlight);

                            final_color = final_color.colorAdd(reflection_intersection_color.colorScalar(0.3));

                        }
                    }
					*/
                }

				 //Beckmann Model
                if ( winning_object_Ambient_Intensity.getColorSpecial() == 9)
                {


                    Vect  LplusVision = light_direction.vectAdd(intersecting_ray_direction.negative().normalize());
                    double Lmagnitude = light_direction.magnitude();
                    double Vmagnitude = intersecting_ray_direction.negative().normalize().magnitude();
                    Vect H = LplusVision.vectMult((1/(Lmagnitude+Vmagnitude)));


                    double NMultH =H.dotProduct(winning_object_normal);

                    double Surface_Roughness=0.9 ;
                    double PI= 3.14;


                    //exp(-tan2(Alpha)/m2)/PI*m2*cos4(Alpha)  Alpha--> Arcos(N.H)
                    float Alpha = acos(NMultH);

                    double Kspec = exp((((-1) * pow(tan(Alpha), 2))/pow(Surface_Roughness, 2))) / ((PI*pow(Surface_Roughness, 2))*(pow(cos(Alpha), 4)));



                    final_color = (Light_Ambient_Intensity.colorMultiply(winning_object_Ambient_Intensity)).colorAdd( (Light_Diffuse_Intensity.colorMultiply(winning_object_Diffuse_Intensity)).colorScalar(light_direction.dotProduct(winning_object_normal)).colorAdd((Light_Specular_Intensity.colorMultiply(winning_object_Specular_Intensity))).colorScalar(Kspec));


                }

				 //Cook_Torrance model
                if ( winning_object_Ambient_Intensity.getColorSpecial() == 10){
                    Vect  LplusVision = light_direction.vectAdd(intersecting_ray_direction.negative().normalize());
                    double Lmagnitude = light_direction.magnitude();
                    double Vmagnitude = intersecting_ray_direction.negative().normalize().magnitude();
                    Vect H = LplusVision.vectMult((1/(Lmagnitude+Vmagnitude)));
                    float CosineTheta =H.dotProduct(winning_object_normal);
                    float Surface_roughness=0.3;
                    double PI= 3.14;
                    float Alpha = acos(CosineTheta);
                    //Beckmann Term
                    // exp(-tan2(Alpha)/m2)/PI*m2*cos4(Alpha)      Alpha--> Arcos(N.H) BeckmannDistribution
                    float Beckmann_Distribution = exp((((-1) * pow(tan(Alpha), 2))/pow(Surface_roughness, 2))) / ((PI*pow(Surface_roughness, 2))*(pow(cos(Alpha), 4)));
                    //Fresnel Term
                    //  F=F0+(1−Fo)(1−cos(θ))5
                    //Refraction_Index depends on the material type
                    // float n = 0.084;
                    //normal reflactance
                    float FO = 1.1;//pow((n-1)/(n+1), 2);
                    //  F = F0 * (1-F0) +puissance(1-H °V)5
                    float Fresnel_Term = FO + (1-FO) * pow(1-H.dotProduct(intersecting_ray_direction.negative()),5);
                    //   double c =intersecting_ray_direction.negative().normalize().dotProduct(H);
                    // double g = sqrt(pow(n,2)+pow(c, 2)-1);
                    // float Fresnel_Term =(1/2) * ( (pow(g-c, 2)/(pow(g+c,2))) * ( 1 + ( pow((c * (g+c) -1),2)/pow((c * (g+c) +1),2))));
                    //Geometry attuenation
                    //calculating the min value
                   // compare between 1 , (2 * (H°N)(V°N) / (V°H)) , (2*(H°N)(L°N)/(V°H))
                   double second = (2) * (winning_object_normal.dotProduct(H)) * (winning_object_normal.dotProduct(intersecting_ray_direction.negative())) / ( (intersecting_ray_direction.negative().dotProduct(H)));
                    double third = (2) * (winning_object_normal.dotProduct(H)) *(winning_object_normal.dotProduct(light_direction)) / ( (light_direction.dotProduct(H)));
                    double Geometry;
                    if (second < third && second < 1 ) {
                       Geometry = second;
                    }
                    if (third < second && third < 1 ) {
                       Geometry = third;
                   }
                    if (1<second && 1<third) {
                       Geometry = 1;
                    }
                    //Calculatin CookTorrance Kspec
                    // CoockTorrance = D*F*G / 4 * (V°N) (N°L)
                    double Kspec = (Beckmann_Distribution * Fresnel_Term * Geometry ) / ((4) * (winning_object_normal.dotProduct(intersecting_ray_direction.negative().normalize())) * ( light_direction.dotProduct(winning_object_normal)  ));
                    //  = (Kd*Idr*(N.L)+Ks*Is*Kspec) ;
                    final_color = winning_object_Ambient_Intensity.colorMultiply(Light_Ambient_Intensity).colorAdd(winning_object_Diffuse_Intensity.colorMultiply(Light_Diffuse_Intensity).colorScalar(winning_object_normal.dotProduct(light_direction))).colorAdd(winning_object_Specular_Intensity.colorMultiply(Light_Specular_Intensity).colorScalar(Kspec));
					/*
                    // reflection from objects with specular intensity
                    double dot1 = winning_object_normal.dotProduct(intersecting_ray_direction.negative());
                    Vect scalar1 = winning_object_normal.vectMult(dot1);
                    Vect add1 = scalar1.vectAdd(intersecting_ray_direction);
                    Vect scalar2 = add1.vectMult(2);
                    Vect add2 = intersecting_ray_direction.negative().vectAdd(scalar2);
                    Vect reflection_direction = add2.normalize();


                    Ray reflection_ray(intersection_position, reflection_direction);

                    //determine what the ray intersects with first
                    vector<double> reflection_intersections;

                    for (int reflection_index = 0; reflection_index < scene_objects.size(); reflection_index++){

                        reflection_intersections.push_back(scene_objects.at(reflection_index)->findIntersection(reflection_ray));

                    }

                    int index_of_winning_object_with_reflection = winningObjectIndex(reflection_intersections);

                    if(index_of_winning_object_with_reflection != -1){

                        //reflection ray missed everything else

                        if(reflection_intersections.at(index_of_winning_object_with_reflection) > accuracy){

                            // determine the position and direction at the point of intersection with the reflection
                            //the ray only affects the color if it reflected off something

                            Vect reflection_intersection_position = intersection_position.vectAdd(reflection_direction.vectMult(reflection_intersections.at(index_of_winning_object_with_reflection)));
                            Vect reflection_intersecting_ray_direction = reflection_direction;


                            Color reflection_intersection_color = getColorAt(reflection_intersection_position, reflection_intersecting_ray_direction, scene_objects, index_of_winning_object_with_reflection, light_sources, accuracy, ambientlight);

                            final_color = final_color.colorAdd(reflection_intersection_color.colorScalar(0.4));

                        }

                    }

                    */

                }

				//Torrance && sparrow model
                if ( winning_object_Ambient_Intensity.getColorSpecial() == 11){

                    Vect  LplusVision = light_direction.vectAdd(intersecting_ray_direction.negative().normalize());
                    double Lmagnitude = light_direction.magnitude();
                    double Vmagnitude = intersecting_ray_direction.negative().normalize().magnitude();
                    Vect H = LplusVision.vectMult( (1/(Lmagnitude+Vmagnitude)));
                    float Smoothless_Value=0.3;
                    double PI= 3.14;					
                    float cosine_Angle =H.dotProduct(winning_object_normal);
                    float Angle = acos(cosine_Angle);
                    float Auto_Correlation = 2;
                    //D = exp(-(Angle entre  N,H)/m)2
                    double GaussianDistribution = Auto_Correlation * exp((-1) * pow(Angle/Smoothless_Value, 2));															
                    //Fresnel Term					
                    //  F=F0+(1−Fo)(1−cos(θ))5
                    //float n = 90;
                    // float FO = abs(pow((n1-n2_Refraction_Index)/(n1+n2_Refraction_Index), 2));
                    float FO =1.5 ; //pow((n-1)/(n+1), 2);				
                    float Fresnel_Term = FO + (1-FO) * pow(1-H.dotProduct(intersecting_ray_direction.negative()),5);			
                    //Geometry attuenation
                    //calculating the min value
                    // compare between 1 , (2 * (H°N)(V°N) / (V°H)) , (2*(H°N)(L°N)/(V°L))
                    double second = (2) * (winning_object_normal.dotProduct(H)) * (winning_object_normal.dotProduct(intersecting_ray_direction.negative())) / ( (intersecting_ray_direction.negative().dotProduct(H)));

                    double third = (2) * (winning_object_normal.dotProduct(H)) *(winning_object_normal.dotProduct(light_direction)) / ( (light_direction.dotProduct(H)));

                    double Geometry = 0.0 ;
                    if (second < third && second < 1 ) {
                        Geometry = second;			
                    }
                    if (third < second && third < 1 ) {
                        Geometry = third;
                    }

                    if (1<second && 1<third) {

                        Geometry = 1;
                    }				
                    //Calculatin Torrance and sparrow Ks
                    // Torrance and sparrow = D*F*G / 4 * (V°N) (N°L)
                    double Kspec = (GaussianDistribution * Fresnel_Term * Geometry ) / ((PI) * (winning_object_normal.dotProduct(intersecting_ray_direction.negative().normalize())) * ( light_direction.dotProduct(winning_object_normal)  ));
                    //  = (Kd*Idr*(N.L)+Ks*Is*Kspec) ;
                    final_color = winning_object_Diffuse_Intensity.colorMultiply(Light_Diffuse_Intensity).colorScalar(winning_object_normal.dotProduct(light_direction)).colorAdd(winning_object_Specular_Intensity.colorMultiply(Light_Specular_Intensity).colorScalar(Kspec));
					
                    //  = (Kd*Idr*(N.L)+Ks*Is*Kspec) ;

                    final_color = winning_object_Ambient_Intensity.colorMultiply(Light_Ambient_Intensity).colorAdd(winning_object_Diffuse_Intensity.colorMultiply(Light_Diffuse_Intensity).colorScalar(winning_object_normal.dotProduct(light_direction))).colorAdd(winning_object_Specular_Intensity.colorMultiply(Light_Specular_Intensity).colorScalar(Kspec));
					/*
                    // reflection from objects with specular intensity
                    double dot1 = winning_object_normal.dotProduct(intersecting_ray_direction.negative());
                    Vect scalar1 = winning_object_normal.vectMult(dot1);
                    Vect add1 = scalar1.vectAdd(intersecting_ray_direction);
                    Vect scalar2 = add1.vectMult(2);
                    Vect add2 = intersecting_ray_direction.negative().vectAdd(scalar2);
                    Vect reflection_direction = add2.normalize();


                    Ray reflection_ray(intersection_position, reflection_direction);

                    //determine what the ray intersects with first
                    vector<double> reflection_intersections;

                    for (int reflection_index = 0; reflection_index < scene_objects.size(); reflection_index++){

                        reflection_intersections.push_back(scene_objects.at(reflection_index)->findIntersection(reflection_ray));

                    }

                    int index_of_winning_object_with_reflection = winningObjectIndex(reflection_intersections);

                    if(index_of_winning_object_with_reflection != -1){

                        //reflection ray missed everything else

                        if(reflection_intersections.at(index_of_winning_object_with_reflection) > accuracy){

                            // determine the position and direction at the point of intersection with the reflection
                            //the ray only affects the color if it reflected off something

                            Vect reflection_intersection_position = intersection_position.vectAdd(reflection_direction.vectMult(reflection_intersections.at(index_of_winning_object_with_reflection)));
                            Vect reflection_intersecting_ray_direction = reflection_direction;


                            Color reflection_intersection_color = getColorAt(reflection_intersection_position, reflection_intersecting_ray_direction, scene_objects, index_of_winning_object_with_reflection, light_sources, accuracy, ambientlight);

                            final_color = final_color.colorAdd(reflection_intersection_color.colorScalar(0.5));

                        }

                    }
					
					*/

                }

				//next Model 

				 //TROWbridge et reitz model

				if ( winning_object_Ambient_Intensity.getColorSpecial() == 12){


                    Vect  LplusVision = light_direction.vectAdd(intersecting_ray_direction.negative().normalize());
                    double Lmagnitude = light_direction.magnitude();
                    double Vmagnitude = intersecting_ray_direction.negative().normalize().magnitude();
                    Vect H = LplusVision.vectMult((1/(Lmagnitude+Vmagnitude)));


                    float CosineTheta = H.dotProduct(winning_object_normal);

                    float Surface_roughness=0.4;

                    double PI= 3.14;


                    float Alpha = acos(CosineTheta);


                    //Trow Term

                    float trow = pow((pow(Surface_roughness,2)/((pow(Surface_roughness, 2)-1) * pow(CosineTheta, 2) +1)),2) ;
                    

                    //Fresnel Term
                    //  F=F0+(1−Fo)(1−cos(θ))5
                    //Refraction_Index depends on the material type
                    // float n = 20;

                    //normal reflactance
                    float FO =7;// pow((n-1)/(n+1), 2);

                    //  F = F0 * (1-F0) +puissance(1-H °V)5
                    float Fresnel_Term = FO + (1-FO) * pow(1-H.dotProduct(intersecting_ray_direction.negative()),5);

                    //   double c =intersecting_ray_direction.negative().normalize().dotProduct(H);
                    // double g = sqrt(pow(n,2)+pow(c, 2)-1);
                    // float Fresnel_Term =(1/2) * ( (pow(g-c, 2)/(pow(g+c,2))) * ( 1 + ( pow((c * (g+c) -1),2)/pow((c * (g+c) +1),2))));



                    //Geometry attuenation
                    //calculating the min value

                    // compare between 1 , (2 * (H°N)(V°N) / (V°H)) , (2*(H°N)(L°N)/(V°H))

                    double second = (2) * (winning_object_normal.dotProduct(H)) * (winning_object_normal.dotProduct(intersecting_ray_direction.negative())) / ( (intersecting_ray_direction.negative().dotProduct(H)));

                    double third = (2) * (winning_object_normal.dotProduct(H)) *(winning_object_normal.dotProduct(light_direction)) / ( (light_direction.dotProduct(H)));

                    double Geometry;



                    if (second < third && second < 1 ) {

                        Geometry = second;


                    }

                    if (third < second && third < 1 ) {

                        Geometry = third;
                    }

                    if (1<second && 1<third) {

                        Geometry = 1;

                    }

				 //Calculatin trow Kspec

                    // trow = D*F*G / 4 * (V°N) (N°L)
                    double Kspec = (trow * Fresnel_Term * Geometry ) / ((4) * (winning_object_normal.dotProduct(intersecting_ray_direction.negative().normalize())) * ( light_direction.dotProduct(winning_object_normal)  ));
					
                    //  = (Kd*Idr*(N.L)+Ks*Is*Kspec) ;

                    final_color = (Light_Ambient_Intensity.colorMultiply(winning_object_Ambient_Intensity)).colorAdd( winning_object_Diffuse_Intensity.colorMultiply(Light_Diffuse_Intensity).colorScalar(winning_object_normal.dotProduct(light_direction)).colorScalar(1/PI).colorAdd(winning_object_Specular_Intensity.colorMultiply(Light_Specular_Intensity).colorScalar(Kspec)));

					/*
                    // reflection from objects with specular intensity
                    double dot1 = winning_object_normal.dotProduct(intersecting_ray_direction.negative());
                    Vect scalar1 = winning_object_normal.vectMult(dot1);
                    Vect add1 = scalar1.vectAdd(intersecting_ray_direction);
                    Vect scalar2 = add1.vectMult(2);
                    Vect add2 = intersecting_ray_direction.negative().vectAdd(scalar2);
                    Vect reflection_direction = add2.normalize();


                    Ray reflection_ray(intersection_position, reflection_direction);

                    //determine what the ray intersects with first
                    vector<double> reflection_intersections;

                    for (int reflection_index = 0; reflection_index < scene_objects.size(); reflection_index++){

                        reflection_intersections.push_back(scene_objects.at(reflection_index)->findIntersection(reflection_ray));

                    }

                    int index_of_winning_object_with_reflection = winningObjectIndex(reflection_intersections);

                    if(index_of_winning_object_with_reflection != -1){

                        //reflection ray missed everything else

                        if(reflection_intersections.at(index_of_winning_object_with_reflection) > accuracy){

                            // determine the position and direction at the point of intersection with the reflection
                            //the ray only affects the color if it reflected off something

                            Vect reflection_intersection_position = intersection_position.vectAdd(reflection_direction.vectMult(reflection_intersections.at(index_of_winning_object_with_reflection)));
                            Vect reflection_intersecting_ray_direction = reflection_direction;


                            Color reflection_intersection_color = getColorAt(reflection_intersection_position, reflection_intersecting_ray_direction, scene_objects, index_of_winning_object_with_reflection, light_sources, accuracy, ambientlight);

                            final_color = final_color.colorAdd(reflection_intersection_color.colorScalar(0.3));

                        }
                    }
								*/							
                }
				//next model 

				//Schlick Model 
				if(winning_object_Ambient_Intensity.getColorSpecial() >= 13 &&  winning_object_Ambient_Intensity.getColorSpecial() <14){


                    // reflection direction =light direction −2( ligh direction ⋅  normal ) normal

                    double LMultNormal = 2 * (light_direction.dotProduct(winning_object_normal));
                    Vect New = winning_object_normal.vectMult(LMultNormal);
                    Vect RD = light_direction.vectSous(New);

                    double Shininess = 9;

                    double Kspec = ((RD.dotProduct(intersecting_ray_direction)/(Shininess - ((Shininess-1) * RD.dotProduct(intersecting_ray_direction)))));


                    final_color = winning_object_Ambient_Intensity.colorMultiply(Light_Ambient_Intensity).colorAdd(winning_object_Diffuse_Intensity.colorMultiply(Light_Diffuse_Intensity).colorScalar(winning_object_normal.dotProduct(light_direction))).colorAdd(
                                                                                                                                                                                                                                                                    winning_object_Specular_Intensity.colorMultiply(Light_Specular_Intensity).colorScalar(Kspec));
					/*
                    // reflection from objects with specular intensity
                    double dot1 = winning_object_normal.dotProduct(intersecting_ray_direction.negative());
                    Vect scalar1 = winning_object_normal.vectMult(dot1);
                    Vect add1 = scalar1.vectAdd(intersecting_ray_direction);
                    Vect scalar2 = add1.vectMult(2);
                    Vect add2 = intersecting_ray_direction.negative().vectAdd(scalar2);
                    Vect reflection_direction = add2.normalize();


                    Ray reflection_ray(intersection_position, reflection_direction);

                    //determine what the ray intersects with first
                    vector<double> reflection_intersections;

                    for (int reflection_index = 0; reflection_index < scene_objects.size(); reflection_index++){

                        reflection_intersections.push_back(scene_objects.at(reflection_index)->findIntersection(reflection_ray));

                    }

                    int index_of_winning_object_with_reflection = winningObjectIndex(reflection_intersections);

                    if(index_of_winning_object_with_reflection != -1){

                        //reflection ray missed everything else

                        if(reflection_intersections.at(index_of_winning_object_with_reflection) > accuracy){

                            // determine the position and direction at the point of intersection with the reflection
                            //the ray only affects the color if it reflected off something

                            Vect reflection_intersection_position = intersection_position.vectAdd(reflection_direction.vectMult(reflection_intersections.at(index_of_winning_object_with_reflection)));
                            Vect reflection_intersecting_ray_direction = reflection_direction;


                            Color reflection_intersection_color = getColorAt(reflection_intersection_position, reflection_intersecting_ray_direction, scene_objects, index_of_winning_object_with_reflection, light_sources, accuracy, ambientlight);

                            final_color = final_color.colorAdd(reflection_intersection_color.colorScalar(0.3));

                        }
                    }
					*/
                }

				//Lewis model

				if(winning_object_Ambient_Intensity.getColorSpecial() >= 15 &&  winning_object_Ambient_Intensity.getColorSpecial() <16){
							
                    Vect  LplusVision = light_direction.vectAdd(intersecting_ray_direction.negative());
                    float Lmagnitude = light_direction.magnitude();
                    float Vmagnitude = intersecting_ray_direction.negative().normalize().magnitude();
                    Vect H = LplusVision.normalize();
                    double PI = 3.14;				
                    double Shininess = 9;
                    double Kspec = ((Shininess+2)/(2 * PI)) * pow(winning_object_normal.dotProduct(H),Shininess);
                    final_color = (Light_Ambient_Intensity.colorMultiply(winning_object_Ambient_Intensity)).colorAdd( (Light_Diffuse_Intensity.colorMultiply(winning_object_Diffuse_Intensity)).colorScalar(light_direction.dotProduct(winning_object_normal))).colorAdd((Light_Specular_Intensity.colorMultiply(winning_object_Specular_Intensity)).colorScalar(Kspec));


					/*
                    // reflection from objects with specular intensity
                    double dot1 = winning_object_normal.dotProduct(intersecting_ray_direction.negative());
                    Vect scalar1 = winning_object_normal.vectMult(dot1);
                    Vect add1 = scalar1.vectAdd(intersecting_ray_direction);
                    Vect scalar2 = add1.vectMult(2);
                    Vect add2 = intersecting_ray_direction.negative().vectAdd(scalar2);
                    Vect reflection_direction = add2.normalize();


                    Ray reflection_ray(intersection_position, reflection_direction);

                    //determine what the ray intersects with first
                    vector<double> reflection_intersections;

                    for (int reflection_index = 0; reflection_index < scene_objects.size(); reflection_index++){

                        reflection_intersections.push_back(scene_objects.at(reflection_index)->findIntersection(reflection_ray));

                    }

                    int index_of_winning_object_with_reflection = winningObjectIndex(reflection_intersections);

                    if(index_of_winning_object_with_reflection != -1){

                        //reflection ray missed everything else

                        if(reflection_intersections.at(index_of_winning_object_with_reflection) > accuracy){

                            // determine the position and direction at the point of intersection with the reflection
                            //the ray only affects the color if it reflected off something

                            Vect reflection_intersection_position = intersection_position.vectAdd(reflection_direction.vectMult(reflection_intersections.at(index_of_winning_object_with_reflection)));
                            Vect reflection_intersecting_ray_direction = reflection_direction;


                            Color reflection_intersection_color = getColorAt(reflection_intersection_position, reflection_intersecting_ray_direction, scene_objects, index_of_winning_object_with_reflection, light_sources, accuracy, ambientlight);

                            final_color = final_color.colorAdd(reflection_intersection_color.colorScalar(0.3));

                        }
                    }
					*/
                }
				
				//Ashikhmin model
				if ( winning_object_Ambient_Intensity.getColorSpecial() >= 17 &&  winning_object_Ambient_Intensity.getColorSpecial() <18){

                    float PI = 3.14;
                    // reflection direction =light direction −2( ligh direction ⋅  normal ) normal

                    double LMultNormal = 2 * (light_direction.dotProduct(winning_object_normal));
                    Vect New = winning_object_normal.vectMult(LMultNormal);
                    Vect RD = light_direction.vectSous(New);


                    Vect  LplusVision = light_direction.vectAdd(intersecting_ray_direction.negative().normalize());

                    float Lmagnitude = light_direction.magnitude();

                    float Vmagnitude = intersecting_ray_direction.negative().normalize().magnitude();


                    Vect H = LplusVision.vectMult( (1/(Lmagnitude+Vmagnitude)));
                    Vect Direction = Vect(0.0,1.0, 0.0);



                    Direction = Direction.normalize();

                    Vect x = winning_object_normal.crossProduct(Direction);
                    x = x.normalize();

                    Vect y = winning_object_normal.crossProduct(x);
                    y = y.normalize();

                    float Cosine_Incoming_Angle = light_direction.dotProduct(winning_object_normal);

                    float Cosine_Outgoing_Angle = winning_object_normal.dotProduct(intersecting_ray_direction.negative().normalize());

                    float Alpha = acos(winning_object_normal.dotProduct(H));

                    double Surface_Roughnessx;

                    double Surface_Roughnessy;


                    if (winning_object_Ambient_Intensity.getColorSpecial() == 17)
                    {
                        Surface_Roughnessx=60;

                        Surface_Roughnessy=20;
                    }



                    if (winning_object_Ambient_Intensity.getColorSpecial() == 17.1)
                    {

                        Surface_Roughnessx=1000;

                        Surface_Roughnessy=300;
                    }
										
                    float FO =2.1;// pow((n-1)/(n+1), 2);

                    //  F = F0 * (1-F0) +puissance(1-H °V)5
                    float Fresnel_Term = FO + (1-FO) * pow(1-H.dotProduct(intersecting_ray_direction.negative()),5);


                    float First_Term = (sqrtf((Surface_Roughnessx+1)*(Surface_Roughnessy+1)))/(8 * PI);
										
                    float Chosen_Cosine;

                    if (winning_object_normal.dotProduct(RD) > winning_object_normal.dotProduct(light_direction))
                    {
                        Chosen_Cosine = winning_object_normal.dotProduct(intersecting_ray_direction.negative());
                    }

                    if (winning_object_normal.dotProduct(RD) < winning_object_normal.dotProduct(light_direction))
                    {
                        Chosen_Cosine = winning_object_normal.dotProduct(light_direction);
                    }

                    float deviation =0;;
                    float n = ((Surface_Roughnessx * pow(H.dotProduct(x)+deviation,2))+(Surface_Roughnessy * pow(H.dotProduct(y)+deviation,2)))/(1 - pow(H.dotProduct(winning_object_normal),2));

                   // float Third_Term = (pow(winning_object_normal.dotProduct(H)+deviation,n))/(H.dotProduct(intersecting_ray_direction.negative())+deviation*Chosen_Cosine);

                    deviation+=0.1;


                 //   double Kspec =  First_Term * Third_Term * Fresnel_Term;



                    if (winning_object_Ambient_Intensity.getColorSpecial() == 17){

                     //   final_color = winning_object_Ambient_Intensity.colorMultiply(Light_Ambient_Intensity).colorAdd(winning_object_Diffuse_Intensity.colorMultiply(Light_Diffuse_Intensity).colorScalar(winning_object_normal.dotProduct(light_direction))).colorAdd(
//                                                                                                                                                                                                                                                                      winning_object_Specular_Intensity.colorMultiply(Light_Specular_Intensity).colorScalar(Kspec));

                    }


                    if (winning_object_Ambient_Intensity.getColorSpecial() == 17.1)
                    {
//                        final_color = winning_object_Ambient_Intensity.colorMultiply(Light_Ambient_Intensity).colorAdd(winning_object_Diffuse_Intensity.colorMultiply(Light_Diffuse_Intensity).colorScalar(winning_object_normal.dotProduct(light_direction))).colorAdd(
  //                                                                                                                                                                                                                                                                      winning_object_Specular_Intensity.colorMultiply(Light_Specular_Intensity).colorScalar(Kspec));

                    }

					/*

                    // reflection from objects with specular intensity
                    double dot1 = winning_object_normal.dotProduct(intersecting_ray_direction.negative());
                    Vect scalar1 = winning_object_normal.vectMult(dot1);
                    Vect add1 = scalar1.vectAdd(intersecting_ray_direction);
                    Vect scalar2 = add1.vectMult(2);
                    Vect add2 = intersecting_ray_direction.negative().vectAdd(scalar2);
                    Vect reflection_direction = add2.normalize();


                    Ray reflection_ray(intersection_position, reflection_direction);

                    //determine what the ray intersects with first
                    vector<double> reflection_intersections;

                    for (int reflection_index = 0; reflection_index < scene_objects.size(); reflection_index++){

                        reflection_intersections.push_back(scene_objects.at(reflection_index)->findIntersection(reflection_ray));

                    }

                    int index_of_winning_object_with_reflection = winningObjectIndex(reflection_intersections);

                    if(index_of_winning_object_with_reflection != -1){

                        //reflection ray missed everything else

                        if(reflection_intersections.at(index_of_winning_object_with_reflection) > accuracy){

                            // determine the position and direction at the point of intersection with the reflection

                            Vect reflection_intersection_position = intersection_position.vectAdd(reflection_direction.vectMult(reflection_intersections.at(index_of_winning_object_with_reflection)));
                            Vect reflection_intersecting_ray_direction = reflection_direction;


                            Color reflection_intersection_color = getColorAt(reflection_intersection_position, reflection_intersecting_ray_direction, scene_objects, index_of_winning_object_with_reflection, light_sources, accuracy, ambientlight);

                            if(winning_object_Ambient_Intensity.getColorSpecial() == 14){


                                final_color = final_color.colorAdd(reflection_intersection_color.colorScalar(0.3));

                            }


                            if(winning_object_Ambient_Intensity.getColorSpecial() == 14.1){


                                final_color = final_color.colorAdd(reflection_intersection_color.colorScalar(0.6));

                            }

                        }

                    }

					*/

                }



				//next 

				//Oren-Nayer Model 
				if ( winning_object_Ambient_Intensity.getColorSpecial() == 19){

					Vect  LplusVision = light_direction.vectAdd(intersecting_ray_direction.negative().normalize());
                    double Lmagnitude = light_direction.magnitude();
                    double Vmagnitude = intersecting_ray_direction.negative().normalize().magnitude();
                    Vect H = LplusVision.vectMult( (1/(Lmagnitude+Vmagnitude)));

					float LdotN = light_direction.dotProduct(winning_object_normal);


					float cosin_angle =  H.dotProduct(winning_object_normal);
					float angle = acos(cosine_angle);
					float Roughness = 1 ; 

					/*					
					float sigma = exp((-1) * pow(angle/Roughness,2));

					float A = 1 - 0.5 * (pow(sigma,2)/(pow(sigma,2)+0.57));
					float B = 0.45 * (pow(sigma,2)/(pow(sigma,2)+0.09));
					*/

					float A = 1 - ((0.5 *pow(Roughness,2)) / (pow(Roughness,2) +0.33));
					float B = (0.45 * pow(Roughness,2))  / (pow(Roughness,2) + 0.09);


					float AngleViewNormal = acos(intersecting_ray_direction.negative().normalize().dotProduct(winning_object_normal));
					float AngleLightNormal = acos(light_direction.dotProduct(winning_object_normal));
					float AngleNormalLight = acos(winning_object_normal.dotProduct(light_direction));

					float Alpha  = MAX(AngleViewNormal,AngleLightNormal);
					float beta = MIN(AngleViewNormal,AngleLightNormal);

					float Gama = intersecting_ray_direction.negative().normalize().vectSous(winning_object_normal).vectMult(winning_object_normal.dotProduct(intersecting_ray_direction.negative().normalize())).dotProduct(light_direction.vectSous(winning_object_normal).vectMult(winning_object_normal.dotProduct(light_direction)));

				
					float orenayer = (AngleNormalLight) * ((A) +(B * MAX(0,Gama) * sin(Alpha) * tan(beta)));

					final_color = final_color.colorAdd(winning_object_Diffuse_Intensity.colorMultiply(Light_Diffuse_Intensity)).colorScalar(orenayer);

					

				}

				//next Model
				//Minnaert Model 

				if ( winning_object_Ambient_Intensity.getColorSpecial() == 20){

					float VdotN= intersecting_ray_direction.negative().normalize().dotProduct(winning_object_normal);
					float LdotN= light_direction.dotProduct(winning_object_normal);

					float irradience = MAX(0,LdotN);
					float power = 0.8 ;

					float minnaert = pow(VdotN*LdotN,power);

					final_color = final_color.colorAdd(winning_object_Diffuse_Intensity.colorMultiply(Light_Diffuse_Intensity)).colorScalar(minnaert);


				}

				
				       //Phong blinn Model
                if ( winning_object_Ambient_Intensity.getColorSpecial() == 21)
                {

                    //H = (L + V ) / |L + V|
                    // H = ( L + V) * (1/|L+V|) there is no subdivision between vectors

                    Vect  LplusVision = light_direction.vectAdd(intersecting_ray_direction.negative().normalize());
                    double Lmagnitude = light_direction.magnitude();
                    double Vmagnitude = intersecting_ray_direction.negative().normalize().magnitude();
                    Vect H = LplusVision.vectMult( (1/(Lmagnitude+Vmagnitude)));


                    // I = Ia*Ka + Id*Kd(N . L) + Is*Ks(N . H)coeff

                    final_color = (Light_Ambient_Intensity.colorMultiply(winning_object_Ambient_Intensity)).colorAdd( (Light_Diffuse_Intensity.colorMultiply(winning_object_Diffuse_Intensity)).colorScalar(light_direction.dotProduct(winning_object_normal)).colorAdd((Light_Specular_Intensity.colorMultiply(winning_object_Specular_Intensity))).colorScalar(pow(H.dotProduct(winning_object_normal) , 9)));
				}

				//next model
				}

				
			}
			
		}
	}
	
	return final_color.clip();
}

int thisone;

int main (int argc, char *argv[]) {
	cout << "rendering ..." << endl;
	
	clock_t t1, t2;
	t1 = clock();
	
	const int dpi = 72;
	int width = 640;
	int height = 480;
	int n = width*height;
	RGBType *pixels = new RGBType[n];

	vector<Object*> scene_objects;
	
    const  int aadepth = 1;
	double aathreshold = 0.1;
	double aspectratio = (double)width/(double)height;
	double ambientlight = 0.2;
	double accuracy = 0.00000001;
	/*
Lampert Model  == 3
Phong Model == 4
Ward Isotropic Model == 5
Ward Anisotropic Model == 6 7 
Gaussian Model == 8
Beckmann Model == 9
Cook Torrance Model == 10
Torrance && Sparrow model == 11
TROWbridge et reitz model ==  12
Schlick Model  == 13 14
Lewis model == 15 16
Ashikhmin model == 17 18 
Oren-Nayer Model == 19
Minnaert Model  == 20
PHONG BLINN = 21




*/
	Vect O (0,0,0);
	Vect X (1,0,0);
	Vect Y (0,1,0);
	Vect Z (0,0,1);
	
	Vect new_sphere_location (1.75, -0.25, 0);
	
	Vect campos (3, 1.5, -4);
	//Vect campos (0, 0, -4);

	Vect look_at (0, 0, 0);
	Vect diff_btw (campos.getVectX() - look_at.getVectX(), campos.getVectY() - look_at.getVectY(), campos.getVectZ() - look_at.getVectZ());
	
	Vect camdir = diff_btw.negative().normalize();
	Vect camright = Y.crossProduct(camdir).normalize();
	Vect camdown = camright.crossProduct(camdir);
	Camera scene_cam (campos, camdir, camright, camdown);
	
	Color white_light (1.0, 1.0, 1.0, 0);
	Color pretty_green (0.5, 1.0, 0.5, 0.3);
	Color maroon (0.5, 0.25, 0.25, 0);
	Color tile_floor (1, 1, 1, 2);
	Color gray (0.5, 0.5, 0.5, 0); 
	Color black (0.0, 0.0, 0.0, 0);

	//speculaire

	Color Refraction_Material_Specular(1.0  , 1.0, 1.0,3);
	Color Steel_Material_Specular( 0.6748 ,0.6748,   0.6748,0);
	Color Material_Specular ( 1, 1, 1, 0 );
	Color Gold_Material_Specular ( 0.628281, 0.555802, 0.366065, 0 );
	Color Brass_Material_Specular( 0.992157 ,0.941176,    0.807843,0);
	Color Silver_Material_Specular ( 0.508273 , 0.508273  , 0.508273, 0 );
	Color Copper_Material_Specular ( 0.992157 ,0.941176,    0.807843,0);
    Color Pearl_Material_Specular (0.296648   , 0.296648  ,  0.296648,0 );
    Color Chrome_Material_Specular (0.774597 ,   0.774597  ,  0.774597,0 );
    Color Bronze_Material_Specular (0.393548  ,  0.271906  ,  0.166721,0);
	Color Ashikhmin_Material_Specular( 0.296648 ,   0.296648  ,  0.296648,0);
    Color Lewis_Material_Specular (0.727811   , 0.626959,    0.626959,0);
    Color Schlick_Material_Specular(0.04  ,  0.7, 0.7,0);
	Color Wood_semi(0.97,0.12,0.099,0);


	//Ambient And Diffuse 
    Color Lambert_Material_Ambient_R(0.3486 , 0.0 ,0.0, 3);
   	Color Lambert_Material_Diffuse_R( 1.0, 0.0, 0.0, 3);

	Color Phong_Material_Ambient_B(0.0, 0.0, 0.8, 4);
    Color Phong_Material_Diffuse_B(0.0, 0.0, 1.0, 4);

	Color Ashikhmin_Material_Ambient_Obsidien(0.05375, 0.05  ,  0.06625, 17.1);
    Color Ashikhmin_Material_Diffuse_Obsidien( 0.18275 ,0.17 ,  0.22525 , 17.1);

	Color Ashikhmin_Material_Ambient(0.25    ,0.20725, 0.20725, 17);
    Color Ashikhmin_Material_Diffuse( 0.829,   0.829  , 1         , 17);

    Color Schlick_Material_Ambient_Obsidien(0   ,0.05  ,  0.05, 13);
    Color Schlick_Material_Diffuse_Obsidien(0.4 ,0.5, 0.5, 13);
		
    Color Lewis_Material_Ambient(0.1745 , 0.01175, 0.01175 ,15);
    Color Lewis_Material_Diffuse(  0.61424 ,0.04136 ,0.04136,15);

	Color Ward_istropic_Material_Ambient_Sunset(   0.329412   , 0.223529 ,   0.027451   ,5);
    Color Ward_istropic_Material_Diffuse_Sunset(  0.780392 ,   0.568627   , 0.113725    , 5);

	Color Ward_Anistropic_Material_Ambient_Sunset(  0.25  ,  0.25  ,  0.25 ,6.5);
    Color Ward_Anistropic_Material_Diffuse_Sunset( 0.4, 0.4 ,0.4 , 6.5);

	Color Ward_Anistropic_Material_Ambient_2(  0.19125, 0.0735 , 0.0225   ,6);
    Color Ward_Anistropic_Material_Diffuse_2( 0.7038 , 0.27048 ,0.0828 , 6);
	
    Color Ward_Anistropic_Material_Ambient_Steel(  0.3 , 0.3 ,0.3   ,6.7);
    Color Ward_Anistropic_Material_Diffuse_Steel( 0.572 , 0.579 ,0.598 , 6.7);

	Color Gaussian_Material_Ambient_R( 0.3486 , 0.0 ,0.0, 8);
    Color Gaussian_Material_Diffuse_R( 1.0, 0.0, 0.0, 8);
	
	Color Beckmann_Material_Ambient_P(  0.1137, 0.1176, 0.2706,9);
    Color Beckmann_Material_Diffuse_P( 0.1804, 0.1882, 0.5608, 9);

	Color Cook_Torrance_Material_Ambient_Sunset(  0.24725 , 0.1995, 0.0745,10);
    Color Cook_Torrance_Material_Diffuse_Sunset( 0.75164, 0.60648 ,  0.22648, 10);

	Color Torrance_Material_Ambient_Sunset(  0.19225 , 0.19225 ,0.19225 ,11);
    Color Torrance_Material_Diffuse_Sunset(  0.50754    , 0.50754   , 0.50754, 11);

	Color Trow_Material_Ambient_Sunset( 0.2125 , 0.1275 , 0.054,12);
    Color Trow_Material_Diffuse_Sunset( 0.714,   0.4284 , 0.18144, 12);

	Color OrenNayar_Material_Ambient_B(0.2039 , 0.3765 ,0.3490, 19);
    Color OrenNayar_Material_Diffuse_B(0.2039 , 0.3765 ,0.3490, 19);
	
	
	             std::vector < glm::vec3 >  out_vertices;
                 std::vector < glm::vec2 >  out_uvs;
                 std::vector < glm::vec3 >  out_normals;
				 
				 
				  loadOBJ("Bunnytest.obj",out_vertices,out_uvs,out_normals);
				 
				 //OBJModel loaderObject("head.obj"); m/ah/och yakhDEM 
				  
			      
	             

		
				  /*
				
				   cout << " Triangles Number: " << out_vertices.size()<< "\n"; 

				   scene_Triangles = Implementing_Triangles(out_vertices,OrenNayar_Material_Ambient_B, OrenNayar_Material_Diffuse_B, Material_Specular);
				  
				 	

				  for (int i=0; i<scene_Triangles.size() ; i++) {
					 printf("number : %d \n",i);
                     scene_objects.push_back(dynamic_cast<Object*>(&scene_Triangles[i]));
                 }
		*/
			
				
	

	// Color used for Light Sources

    Color Light_Source_Ambient(0.4,0.4,0.4,0);
    Color Light_Source_Diffuse(1.0,1.0,1.0,0);
    Color Light_Source_Specular(1.0,1.0,1.0,0);

	Vect light_position (2,10,-10);
	Vect light_position2(2,10,-10);
	Light scene_light (light_position, white_light);
	Light scene_light2(light_position2,Light_Source_Ambient,Light_Source_Diffuse,Light_Source_Specular);
	vector<Source*> light_sources; 
	light_sources.push_back(dynamic_cast<Source*>(&scene_light));
	//light_sources.push_back(dynamic_cast<Source*>(&scene_light2));
	
	
	// scene objects
	Sphere scene_sphere (O, 1, pretty_green);
	Sphere scene_sphere2 (new_sphere_location, 0.5, maroon);
	Sphere scene_sphere3 (O,1,OrenNayar_Material_Ambient_B, OrenNayar_Material_Diffuse_B, Material_Specular);
                                                         
	Vect Y1 (1,0,0);
	Vect Y2 (0,0,1);
	Plane scene_plane (Y, -1, tile_floor);
	Plane scene_plane2 (Y1, -3, maroon);
	Plane scene_plane3 (Y2, -1, maroon);
	Plane scene_plane4 (Y1, 3, maroon);
	
	scene_objects.push_back(dynamic_cast<Object*>(&scene_sphere));
	scene_objects.push_back(dynamic_cast<Object*>(&scene_sphere2));
	scene_objects.push_back(dynamic_cast<Object*>(&scene_plane));
	//scene_objects.push_back(dynamic_cast<Object*>(&scene_plane2));
	//scene_objects.push_back(dynamic_cast<Object*>(&scene_plane3));
	//scene_objects.push_back(dynamic_cast<Object*>(&scene_plane4));
	//scene_objects.push_back(dynamic_cast<Object*>(&scene_sphere3));
	
	
	int thisone, aa_index;
	double xamnt, yamnt;
	double tempRed, tempGreen, tempBlue;
	
	for (int x = 0; x < width; x++) {
		for (int y = 0; y < height; y++) {
			thisone = y*width + x;
			printf("pixel lunched  %d  %d\n",x,y);
			// start with a blank pixel
			
			double tempRed[aadepth*aadepth];
			double tempGreen[aadepth*aadepth];
			double tempBlue[aadepth*aadepth];
#pragma omp parallel
#pragma omp for
			for (int aax = 0; aax < aadepth; aax++) {
				for (int aay = 0; aay < aadepth; aay++) {
			//printf("rays lunched  %d  %d\n",aax,aay);
					aa_index = aay*aadepth + aax;
					
					srand(time(0));
					
					// create the ray from the camera to this pixel
					if (aadepth == 1) {
					
						// start with no anti-aliasing
						if (width > height) {
							// the image is wider than it is tall
							xamnt = ((x+0.5)/width)*aspectratio - (((width-height)/(double)height)/2);
							yamnt = ((height - y) + 0.5)/height;
						}
						else if (height > width) {
							// the imager is taller than it is wide
							xamnt = (x + 0.5)/ width;
							yamnt = (((height - y) + 0.5)/height)/aspectratio - (((height - width)/(double)width)/2);
						}
						else {
							// the image is square
							xamnt = (x + 0.5)/width;
							yamnt = ((height - y) + 0.5)/height;
						}
					}
					else { 

						
						// anti-aliasing
						if (width > height) {
							// the image is wider than it is tall
							xamnt = ((x + (double)aax/((double)aadepth - 1))/width)*aspectratio - (((width-height)/(double)height)/2);
							yamnt = ((height - y) + (double)aax/((double)aadepth - 1))/height;
						}
						else if (height > width) {
							// the imager is taller than it is wide
							xamnt = (x + (double)aax/((double)aadepth - 1))/ width;
							yamnt = (((height - y) + (double)aax/((double)aadepth - 1))/height)/aspectratio - (((height - width)/(double)width)/2);
						}
						else {
							// the image is square
							xamnt = (x + (double)aax/((double)aadepth - 1))/width;
							yamnt = ((height - y) + (double)aax/((double)aadepth - 1))/height;
						}
						
					}
					
					Vect cam_ray_origin = scene_cam.getCameraPosition();
					Vect cam_ray_direction = camdir.vectAdd(camright.vectMult(xamnt - 0.5).vectAdd(camdown.vectMult(yamnt - 0.5))).normalize();
					
					Ray cam_ray (cam_ray_origin, cam_ray_direction);
					
					vector<double> intersections;
					
					for (int index = 0; index < scene_objects.size(); index++) {
						intersections.push_back(scene_objects.at(index)->findIntersection(cam_ray));
					}
					
					int index_of_winning_object = winningObjectIndex(intersections);
					
					if (index_of_winning_object == -1) {
						// set the backgroung black
						tempRed[aa_index] = 0;
						tempGreen[aa_index] = 0;
						tempBlue[aa_index] = 0;
					}
					else{
						// index coresponds to an object in our scene
						if (intersections.at(index_of_winning_object) > accuracy) {
							// determine the position and direction vectors at the point of intersection
							
							Vect intersection_position = cam_ray_origin.vectAdd(cam_ray_direction.vectMult(intersections.at(index_of_winning_object)));
							Vect intersecting_ray_direction = cam_ray_direction;
		
							Color intersection_color = getColorAt(intersection_position, intersecting_ray_direction, scene_objects, index_of_winning_object, light_sources, accuracy, ambientlight);
							
							tempRed[aa_index] = intersection_color.getColorRed();
							tempGreen[aa_index] = intersection_color.getColorGreen();
							tempBlue[aa_index] = intersection_color.getColorBlue();
						}
					}
				}
			}
			
			
			// average the pixel color
			double totalRed = 0;
			double totalGreen = 0;
			double totalBlue = 0;
			
			for (int iRed = 0; iRed < aadepth*aadepth; iRed++) {
				totalRed = totalRed + tempRed[iRed];
			}
			for (int iGreen = 0; iGreen < aadepth*aadepth; iGreen++) {
				totalGreen = totalGreen + tempGreen[iGreen];
			}
			for (int iBlue = 0; iBlue < aadepth*aadepth; iBlue++) {
				totalBlue = totalBlue + tempBlue[iBlue];
			}
			
			double avgRed = totalRed/(aadepth*aadepth);
			double avgGreen = totalGreen/(aadepth*aadepth);
			double avgBlue = totalBlue/(aadepth*aadepth);
			
			pixels[thisone].r = avgRed;
			pixels[thisone].g = avgGreen;
			pixels[thisone].b = avgBlue;
		}
	}
	printf("here clear \n"); 
	savebmp("scene.bmp",width,height,dpi,pixels);
	
	delete pixels, tempRed, tempGreen, tempBlue;
	
	t2 = clock();
	float diff = ((float)t2 - (float)t1)/1000;
	
	cout << diff << " seconds" << endl;
	
	return 0;
}
