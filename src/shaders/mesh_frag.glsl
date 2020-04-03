#version 430

in vec3         normal_out;
in float		color_intp;
in float		alpha;
in vec4         color_val;
in vec3			vposition;
in vec3			eye;
flat in int		otype;
in float             G_ID;

layout (location = 0) out vec4        outcol;
// textures
uniform sampler3D   splat_tex; //r=astro g=astro-mito b=neurite-mito
uniform sampler3D   gly_tex;
uniform sampler1D	gly_tf;

uniform ivec4     splat_flags;
uniform int		  specular_flag;
in vec3             G_fragTexCoord;
uniform int         reset_filter_ssbo;

layout (std430, binding=4) buffer filter_data
{
    vec4 objects_list_filter[];
};

vec3 computeLight(vec3 lightDir, vec3 obj_color);

//-------------------- DIFFUSE LIGHT PROPERTIES --------------------
vec3 lightColor = vec3(1.0, 1.0, 1.0);
vec3 lightDir1 = vec3(-2.5f, -2.5f, -0.9f);
vec3 lightDir2 = vec3(2.5f, 2.5f, 1.0f);
vec3 N = normalize(normal_out);
float k_a = 0.3;
float k_s = 0.5;
// ------------------- TOON SHADER PROPERTIES ----------------------
vec3 E2 = vec3(0.5, 0.5, -1.0);

void main() 
{
	vec3 obj_color = vec3(0.35, 0.52, 0.79);

    vec3 result = computeLight(lightDir1, obj_color);
	result += computeLight(lightDir2, obj_color);
    
	outcol = vec4(result, 1.0);
}

vec3 computeLight(vec3 lightDir1, vec3 obj_color)
{
	vec3 L = normalize(lightDir1);
	
	// ambient component
    vec3 ambient = k_a * lightColor;

	// diffuse component
	float diff = max(dot(N, L), 0.0);
	vec3 diffuse = diff * lightColor;
	
	// specular component
	vec3 viewDir = normalize(eye - vposition);
	vec3 reflectDir = reflect(-L, N);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 8);
	vec3 specular = k_s * spec * lightColor;  

    return (ambient + diffuse + specular) * obj_color;
}

//vec3 getSplattedTexture3(in sampler3D texture_toSplat, in vec3 coord, in vec3 step_size)
//{
//	vec3 idx_offset = vec3(0.0, 0.0, 0.0);
//	float weight = 1;
//	float tex_value = 1.0;
//	float sum = 0;
//	vec3 result = vec3(0);
//	for (int k = 0; k < 5; k++)
//	{
//		idx_offset.z = gaussian_steps[k] * step_size.z;
//		for (int j = 0; j < 5; j++)
//		{
//			idx_offset.y = gaussian_steps[j] * step_size.y;
//			for (int i = 0; i < 5; i++)
//			{
//				idx_offset.x = gaussian_steps[i] * step_size.x;
//				weight = gaussian_kernel[i + j * 5 + k * 25];
//				vec3 new_coord = coord.xyz + idx_offset.xyz;
//				vec4 tex_value4 = texture(texture_toSplat, new_coord);
//				result = result + (weight * tex_value4.rgb);
//			}
//		}
//	}
//	return result;
//}
