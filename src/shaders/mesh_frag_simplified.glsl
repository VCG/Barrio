#version 430
layout (early_fragment_tests) in;

#define MITO  1
#define AXONS 2
#define BOUTN 3
#define DENDS 4
#define SPINE 5
#define ASTRO 6
#define SYNPS 7
#define SLICE 10

#define MAX_FRAGMENTS 25

layout (location = 0) out vec4 FragColor;

in vec4         frag_normal;
in vec4			frag_camera_position;
flat in int     frag_structure_type;
in float        frag_cell_distance;
in vec4         frag_vert_pos;
in flat int     frag_hvgx;
in float        frag_slice_z;
in flat int     frag_is_skeleton;
in flat int     frag_is_overview;
in flat int     frag_currently_hovered_id;

uniform bool          color_code;
uniform int           main_mito;
uniform sampler1D	  mito_colormap;

uniform sampler3D     volume;
uniform bool          showSlice;
uniform bool          show3D;


uniform int           maxNodes;
uniform float         cell_opacity;

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

// ----------------- SSBO data ------------------------------------
layout (std430, binding=5) buffer mesh_data
{
    int SSBO_visibility[];
};

layout (std430, binding=6) buffer highlight_data
{
    int SSBO_highlighted[];
};

subroutine void       RenderPassType();
subroutine uniform    RenderPassType RenderPass;

//-------------------- DIFFUSE LIGHT PROPERTIES --------------------
vec3 lightColor1 = vec3(0.8, 0.8, 0.8);
vec3 lightColor2 = vec3(0.7, 0.7, 0.7);

vec3 lightDir1 = vec3(-2.5f, -2.5f, -0.9f);
vec3 lightDir2 = vec3(2.5f, 2.5f, 1.0f);

float k_a = 0.3;
float k_d = 0.8;
float k_s = 0.5;

vec3 selected_color =  vec3(1.0, 0.65, 0.0); // orange
// vec3 mito_standard = vec3(0.925, 0.341, 0.298); // red
vec3 mito_standard = vec3(0.329, 0.584, 0.941);
vec3 synapse_color = vec3(0.58, 0.0, 0.83); // purple
//vec3 neurite_color = vec3(0.6, 1.0, 0.6); // greenish
vec3 neurite_color = vec3(0.627, 0.870, 0.682);
int isVisible(int hvgx)
{
  if(show3D)
  {
    for(int i = 0; i < SSBO_visibility.length(); i++)
    {
      if(hvgx == SSBO_visibility[i])
      {
        return 1;
      }
    }
  }

  if(showSlice && hvgx == -1)
  {
    return 1;
  }

  return 0;
}

int isHighlighted(int hvgx)
{
  // highlight in overview widget
  if(frag_is_overview == 1 && hvgx == frag_currently_hovered_id)
  {
    return 1;
  }

  for(int i = 0; i < SSBO_highlighted.length(); i++)
  {
    if(hvgx == SSBO_highlighted[i])
    {
      return 1;
    }
  }
  return 0;
}

vec3 computeLight(vec3 light_dir, vec3 light_color, vec3 obj_color)
{
  vec3 L = normalize(light_dir);
  vec3 N = frag_normal.xyz;
  
  // ambient component
  vec3 ambient = k_a * light_color;

  // diffuse component
  float diff = max(dot(N, L), 0.0);
  vec3 diffuse = k_d * diff * light_color;
  
  // specular component
  vec3 viewDir = normalize(frag_camera_position.xyz - frag_vert_pos.xyz);
  vec3 reflectDir = reflect(-L, N);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), 16);
  vec3 specular = k_s * spec * light_color;

  return (ambient + diffuse + specular) * obj_color;
}

vec4 computeColor()
{
  vec3 obj_color;
  vec4 out_color;
  
  if(isHighlighted(frag_hvgx) == 1)
  {
  	obj_color = selected_color;
  }
  else if(frag_structure_type == MITO)
  {
    if(frag_is_skeleton == 0)
    {
      if(color_code)
      {
        obj_color = vec3(texture(mito_colormap, 1 - frag_cell_distance * 4.0).xyz);
      }
      else
      {
        obj_color = mito_standard;
      }
    }
    else
    {
      obj_color = mito_standard;
    }
    
  } 
  else if(frag_structure_type == SYNPS)
  {
    obj_color = synapse_color;
  }
  else if(frag_structure_type == SLICE)
  {
    float depth_normed = frag_slice_z / 5.0;
    obj_color = vec3(texture(volume, vec3(frag_vert_pos.w,  frag_vert_pos.x, depth_normed)).x);
  }
  else if(frag_structure_type == DENDS)
  {
    if(frag_is_skeleton == 1)
    {
      obj_color = mito_standard;
    }
    else
    {
      obj_color = neurite_color;
    }
  } 
  else
  {
    obj_color = neurite_color;
  }

  vec3 result;
  if(frag_structure_type != SLICE && frag_is_skeleton == 0)
  {
    result = computeLight(lightDir1, lightColor1, obj_color);
    result += computeLight(lightDir2, lightColor2, obj_color);

    //vec3 viewDir = normalize(eye_frag.xyz - frag_vert_pos.xyz);
  }
  else
  {
    result = obj_color;
  }

  if(frag_structure_type == MITO && frag_hvgx != main_mito)
  {
    out_color = vec4(result, 0.25);
  }
  else if(frag_structure_type == MITO || frag_structure_type == SYNPS || frag_structure_type == SLICE || isHighlighted(frag_hvgx) == 1 || frag_is_skeleton == 1)
  {
    out_color = vec4(result, 1.0);
  }
  else if(frag_structure_type == DENDS || frag_structure_type == AXONS)
  {
    // compute view dependent transparency based on dot product between viewing ray and surface normal
    vec3 N = normalize(frag_normal.xyz);
    vec3 V = normalize(frag_camera_position.xyz - frag_vert_pos.xyz);
    float opacity = pow(1.0 - abs(dot(N, V)), 2.5) * cell_opacity;
    //out_color = vec4(result, opacity);
    out_color = vec4(result, cell_opacity);
  }

  return out_color;
}


subroutine(RenderPassType) void pass1()
{
  //check visibility
  if(isVisible(frag_hvgx) == 0)
  {
    discard;
  }

  // Get the index of the next empty slot in the buffer
  uint nodeIdx = atomicCounterIncrement(nextNodeCounter);

  // Is our buffer full?  If so, we don't add the fragment to the list.
  if( nodeIdx < maxNodes ) {

    // Our fragment will be the new head of the linked list, so
    // replace the value at gl_FragCoord.xy with our new node's
    // index.  We use imageAtomicExchange to make sure that this
    // is an atomic operation.  The return value is the old head
    // of the list (the previous value), which will become the
    // next element in the list once our node is inserted.
    uint prevHead = imageAtomicExchange(headPointers, ivec2(gl_FragCoord.xy), nodeIdx);

    // Here we set the color and depth of this new node to the color
    // and depth of the fragment.  The next pointer, points to the
    // previous head of the list.
    
    nodes[nodeIdx].color = computeColor();
    nodes[nodeIdx].depth = gl_FragCoord.z;
    nodes[nodeIdx].next = prevHead;
  }
}

  subroutine(RenderPassType) void pass2()
  {
    NodeType frags[MAX_FRAGMENTS];

    int count = 0;

    // Get the index of the head of the list
    uint n = imageLoad(headPointers, ivec2(gl_FragCoord.xy)).r;

    // Copy the linked list for this fragment into an array
    while( n != 0xffffffff && count < MAX_FRAGMENTS) {
      frags[count] = nodes[n];
      n = frags[count].next;
      count++;
    }

  for( uint i = 1; i < count; i++ )
  {
    NodeType toInsert = frags[i];
    uint j = i;
    while( j > 0 && toInsert.depth > frags[j-1].depth ) {
      frags[j] = frags[j-1];
      j--;
    }
    frags[j] = toInsert;
  }


  // Traverse the array, and combine the colors using the alpha
  // channel.
  vec4 color = vec4(1.0, 1.0, 1.0, 1.0);
  for( int i = 0; i < count; i++ )
  {
    color = mix( color, frags[i].color, frags[i].color.a);
  }

  // Output the final color
  FragColor = color;
}

void main()
{
    RenderPass(); 
}