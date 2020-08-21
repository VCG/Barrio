#version 430

/*Order Independent Transperency code is based on the code of liao gang */
// https://github.com/gangliao/Order-Independent-Transparency-GPU


#define MITO  1
#define AXONS 2
#define BOUTN 3
#define DENDS 4
#define SPINE 5
#define ASTRO 6
#define SYNPS 7
#define AMITO 9

#define MAX_FRAGMENTS 75


in vec3         normal_out;
in float		color_intp;
in float		alpha;
in vec4         color_val;
in vec3			vposition;
in vec3			eye;
flat in int		otype;
in float        G_ID;
in float		mito_cell_distance;

layout (location = 0) out vec4        FragColor;
// textures
uniform sampler3D   splat_tex; //r=astro g=astro-mito b=neurite-mito
uniform sampler3D   gly_tex;
uniform sampler1D	gly_tf;
uniform sampler1D	mito_colormap;

uniform ivec4     splat_flags;
uniform int		  specular_flag;
in vec3             G_fragTexCoord;
uniform int         reset_filter_ssbo;

layout (std430, binding=4) buffer filter_data
{
    vec4 objects_list_filter[];
};

vec3 computeLight(vec3 light_dir, vec3 light_color, vec3 obj_color);
float normDistance(float d);

//-------------------- DIFFUSE LIGHT PROPERTIES --------------------
vec3 lightColor1 = vec3(1.0, 1.0, 1.0);
vec3 lightColor2 = vec3(0.7, 0.7, 0.7);

vec3 lightDir1 = vec3(-2.5f, -2.5f, -0.9f);
vec3 lightDir2 = vec3(2.5f, 2.5f, 1.0f);
vec3 N = normalize(normal_out);
float k_a = 0.3;
float k_s = 0.5;

// ------------------- TOON SHADER PROPERTIES ----------------------
vec3 E2 = vec3(0.5, 0.5, -1.0);

// ------------- order independent transparency variables ----------
struct NodeType {
  vec4 color;
  float depth;
  uint next;
};

layout( binding = 0, r32ui) uniform uimage2D headPointers;
layout( binding = 0, offset = 0) uniform atomic_uint nextNodeCounter;
layout( binding = 0, std430 ) buffer linkedLists {
  NodeType nodes[];
};

uniform long maxNodes;
subroutine void RenderPassType();
subroutine uniform RenderPassType RenderPass;


vec3 computeLight(vec3 light_dir, vec3 light_color, vec3 obj_color)
{
	vec3 L = normalize(light_dir);
	
	// ambient component
    vec3 ambient = k_a * light_color;

	// diffuse component
	float diff = max(dot(N, L), 0.0);
	vec3 diffuse = diff * light_color;
	
	// specular component
	vec3 viewDir = normalize(eye - vposition);
	vec3 reflectDir = reflect(-L, N);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 8);
	vec3 specular = k_s * spec * light_color;  

    return (ambient + diffuse + specular) * obj_color;
}

vec4 computeColor()
{
  vec3 obj_color;
  vec4 out_color;

  if(otype != MITO)
  {
	obj_color = vec3(0.6, 1.0, 0.6);
  } 
  else{
    obj_color = texture(mito_colormap, 1 - 2.85 * mito_cell_distance).xyz;
  } 

  vec3 result = computeLight(lightDir1, lightColor1, obj_color);
  result += computeLight(lightDir2, lightColor2, obj_color);

  vec3 viewDir = normalize(eye - vposition);
  float alpha = abs(dot(viewDir, N));

  if(otype == MITO)
  {
    out_color = vec4(result, 1.0);
  }
  else
  {
    out_color = vec4(result, alpha);
  }

  return out_color;
}

// ------------------------------------------------------------------
subroutine(RenderPassType) void pass1()
{  
  uint nodeIdx = atomicCounterIncrement(nextNodeCounter);
  
  if(nodeIdx < maxNodes)
  {
	// get previous head of list
	uint prevHead = imageAtomicExchange(headPointers, ivec2(gl_FragCoord.xy), nodeIdx); // what does this line do exactly
	
	nodes[nodeIdx].color = computeColor();
	nodes[nodeIdx].depth = gl_FragCoord.z; // why this z value? 
	nodes[nodeIdx].next = prevHead;
  }
}

subroutine(RenderPassType) void pass2()
{
    NodeType frags[MAX_FRAGMENTS];
	int count = 0;
	uint list_end = 0xffffffff;
	uint head_idx = imageLoad(headPointers, ivec2(gl_FragCoord.xy)).r; // what does r mean?
	
	while(head_idx != list_end && count < MAX_FRAGMENTS)
	{
	  frags[count] = nodes[head_idx];
	  head_idx = frags[count].next;
	  count++;
	}

	// sort frags based on depth - merge sort
	int i, j1, j2, k;
    int a, b, c;
    int step = 1;
    NodeType leftArray[MAX_FRAGMENTS/2]; //for merge sort
    
    while (step <= count)
    {
        i = 0;
        while (i < count - step)
        {
            a = i;
            b = i + step;
            c = (i + step + step) >= count ? count : (i + step + step);

            for (k = 0; k < step; k++)
                leftArray[k] = frags[a + k];
            
            j1 = 0;
            j2 = 0;
            for (k = a; k < c; k++)
            {
                if (b + j1 >= c || (j2 < step && leftArray[j2].depth > frags[b + j1].depth))
                    frags[k] = leftArray[j2++];
                else
                    frags[k] = frags[b + j1++];
            }
            i += 2 * step;
        }
        step *= 2;
    }
	
    // iterate through sorted map and mix colors and transparencies
	vec4 color = vec4(0.5, 0.5, 0.5, 1.0);
	for( int i = 0; i < count; i++ )
	{
	  color = mix(color, frags[i].color, frags[i].color.a);
	}

	// Output the final color
	FragColor = color;

}

void main() 
{
  RenderPass();
}