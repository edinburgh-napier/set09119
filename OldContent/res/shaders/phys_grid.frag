#version 410

// A directional light structure
struct directional_light
{
	vec4 ambient_intensity;
	vec4 light_colour;
	vec3 light_dir;
};

// A material structure
struct material
{
	vec4 emissive;
	vec4 diffuse_reflection;
	vec4 specular_reflection;
	float shininess;
};

// Directional light for the scene
uniform directional_light light;
// Material of the object
uniform material mat;
// Position of the camera
uniform vec3 eye_pos;
// Texture
//uniform sampler2D tex;

// Incoming position
layout (location = 0) in vec3 position;
// Incoming normal
layout (location = 1) in vec3 normal;
// Incoming texture coordinate
layout (location = 2) in vec2 tex_coord;

// Outgoing colour
layout (location = 0) out vec4 colour;

void main()
{
	// ***************************
	// Calculate ambient component
	// ***************************
	vec4 ambient = mat.diffuse_reflection * light.ambient_intensity;
  float red;
  float blue;
  if(position.x+0.5 < 0){
   red = 1 - (int(position.x+0.5) % 2);
  }else{
   red = int(position.x+0.5) % 2;
  }
  if(position.z+0.5 < 0){
    blue = 1 - (int(position.z+0.5) % 2);
   }else{
   blue = int(position.z+0.5) % 2;
  }
  if((red>0) && (blue>0) || (!(red>0) && !(blue>0))){
   ambient = vec4(0,0,0,0.8);
  }else{
   ambient = vec4(1.0,1.0,1.0,0.8);
  }
	// ***************************
	// Calculate diffuse component
	// ***************************
	vec4 diffuse = (mat.diffuse_reflection * light.light_colour) * max(dot(normal, light.light_dir), 0);
	// ************************
	// Calculate view direction
	// ************************
	vec3 view_dir = normalize(eye_pos - position);
	// *********************
	// Calculate half vector
	// *********************
	vec3 half_vector = normalize(light.light_dir + view_dir);
	// ****************************
	// Calculate specular component
	// ****************************
	vec4 specular = (mat.specular_reflection * light.light_colour) * pow(max(dot(normal, half_vector), 0), mat.shininess);
	// **************
	// Sample texture
	// **************
	//vec4 tex_colour = texture(tex, tex_coord);
	// **********************************
	// Calculate primary colour component
	// **********************************
	vec4 primary = mat.emissive + ambient + diffuse;
	// **********************
	// Calculate final colour
	// - remember alpha
	// **********************
	colour = primary + specular;
  float dist = (length(vec2(position.x,position.z))-10.0f)/30.0f;
	//colour.a = 1.0f;// -((gl_FragCoord.z / gl_FragCoord.w) / 30.0f);
  colour.a = 1.0f - dist;
  //colour.a = 0.6f;
}