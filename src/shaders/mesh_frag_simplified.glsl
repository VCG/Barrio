#version 430
layout (early_fragment_tests) in;

#define MITO  1
#define AXONS 2
#define BOUTN 3
#define DENDS 4
#define SPINE 5
#define ASTRO 6
#define SYNPS 7

#define MAX_FRAGMENTS 50

layout (location = 0) out vec4 FragColor;

in vec4         normal_frag;
in vec3			    eye_frag;
flat in int     frag_structure_type;
in float        hvgx_frag;
in float        frag_cell_distance;

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

uniform int maxNodes;
subroutine void RenderPassType();
subroutine uniform RenderPassType RenderPass;


subroutine(RenderPassType) void pass1()
{
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

    if(frag_structure_type == MITO)
    {
      nodes[nodeIdx].color =  vec4(1.0, 0.0, 0.0, 1.0);
    }
    else
    {
      nodes[nodeIdx].color = vec4(0.0, 1.0, 0.0, 0.05) ;
    }
   
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
  //FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}