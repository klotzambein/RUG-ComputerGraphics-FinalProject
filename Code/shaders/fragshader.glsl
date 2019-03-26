#version 400 core

// Define constants
#define M_PI 3.141593

// temporary fix for OSX
#define texture2D texture

// The input from the vertex shader.
in vec2 uv_in;

// Uniforms
uniform float uAspectRatio;
uniform float uTime;
uniform mat4 uView;

// Lighting model constants.
out vec4 color_out;

#define MAX_STEPS 50
#define MAX_DIST 200.0
#define SURF_DIST 0.01
#define ENABLE_SHADOWS true

float sierpinski(vec3 z) {
    float r;
    int n = 0;
    while (n < 15) {
       if(z.x+z.y < 0) z.xy = -z.yx; // fold 1
       if(z.x+z.z < 0) z.xz = -z.zx; // fold 2
       if(z.y+z.z < 0) z.zy = -z.yz; // fold 3
       z = z * 2 - 1 * (2 - 1.0);
       n++;
    }
    return (length(z) ) * pow(2, -float(n));
}

float mandelbulb(vec3 pos) {
    vec3 z = pos;
    float dr = 1.0;
    float r = 0.0;
    for (int i = 0; i < 4; i++) {
        r = length(z);
        if (r > 10.0) break;
        
        // convert to polar coordinates
        float theta = acos(z.z/r);
        float phi = atan(z.y,z.x);
        dr =  pow( r, 8.0-1.0)*8.0*dr + 1.0;
        
        // scale and rotate the point
        float zr = pow( r,8.0);
        theta = theta*8.0;
        phi = phi*8.0;
        
        // convert back to cartesian coordinates
        z = zr*vec3(sin(theta)*cos(phi), sin(phi)*sin(theta), cos(theta));
        z += pos;
    }
    return 0.5*log(r)*r/dr;
}

float pingPong(float t, float domain) {
    return abs(mod(t, domain * 2) - domain);
}

float getDist2 (vec3 p) {
    float s = 1.5;
    float scale = 1.0;

    for( int i = 0; i < 8; i++)
    {
        p = -1.0 + 2.0*fract(0.5*p+0.5);

        float r2 = dot(p,p);

        float k = s/r2;
        p     *= k;
        scale *= k;
    }
    
    return 0.25 * abs(p.y) / scale;
}

vec2 getDist (vec3 p)
{
    vec4 s = vec4(0, 1, 6, 1); // x y z r

    float domainWidth = 5;
    p.x = pingPong(p.x - domainWidth/2, domainWidth) - domainWidth/2;
    p.z = pingPong(p.z - domainWidth/2, domainWidth) - domainWidth/2;

    // distance to sphere = length of (p - sphere location) - sphere radius
    float sphereDist = mandelbulb(p - vec3(0, 2.0, 0));//length(p - s.xyz) - s.w;
    float planeDist = p.y;

    // give each object an index for coloring later
    if (sphereDist < planeDist)
        if (int(p.x) % int(domainWidth * 2) == 0) // attempt to alternate colors
            return vec2(1.0, sphereDist);
        else
            return vec2(2.0, sphereDist);
    else
        return vec2(0.0, planeDist);
}

float rayMarch(vec3 ro, vec3 rd)
{
    float dO = 0.0;

    for (int i = 0; i < MAX_STEPS; ++i)
    {
        vec3 p = ro + rd * dO; // current position
        float dS = getDist(p).y;
        dO += dS;

        // distance larger than max or surface distance smaller than min, return hit
        if (dO > MAX_DIST || dS < SURF_DIST) break;
    }
    return dO;
}

vec3 getLight(vec3 p)
{
    vec3 lightPos = vec3(0, 20, 10);
    lightPos.xy += vec2 (4. * sin(0.8 * uTime), cos(0.8 * uTime))*4.;

    vec3 l = normalize(lightPos - p); // light - surface

    vec2 d = getDist(p);

    // get angle by taking two points close to each other
    vec3 n = d.y - vec3(getDist(p - vec3(SURF_DIST/2, 0, 0)).y,
                      getDist(p - vec3(0, SURF_DIST/2, 0)).y,
                      getDist(p - vec3(0, 0, SURF_DIST/2)).y);

    n = normalize(n);

    float dif = dot(n, l);

    vec3 col = vec3(0,0,0);

    if (d.x == 1.0)
         col = vec3(0.91,0.32,0.43); //pink
else if (d.x == 2.0)
         col = vec3(0.18,0.74,0.92); //blue
    else
        col = vec3(0.98,0.91,0.87); //desert

    if (ENABLE_SHADOWS)
    {
        float d = rayMarch(p + n * SURF_DIST * 2.0, l);

        // if we hit an object in between the lightpos and the current pos, return shadow
        if (d < length(lightPos - p))
            return col * dif * 0.5;
    }
    return col * clamp(dif, 0.0, 1.0); // return dot product of lightvec and normalvec
}

void main()
{
    vec3 col = vec3(0, 0, 0);

    vec3 ro = (uView * vec4(0, 3, 0, 1)).xyz;

    vec3 rd = (
        uView * vec4(
            normalize(
                vec3(
                    uv_in.x * uAspectRatio - uAspectRatio / 2,
                    uv_in.y - 0.5,
                    1
                )
            ),
            0)
        )
        .xyz;
 
    float d = rayMarch(ro, rd);

    vec3 p = ro + rd * d; // calculate position for shading

    col = getLight(p);
    
    color_out = vec4(col, 1.0);
}
