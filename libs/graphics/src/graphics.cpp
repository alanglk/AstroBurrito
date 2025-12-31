
#include "astro/graphics/graphics.hpp"
#include "astro/math/math.hpp"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <utility>

using namespace astro::math;

namespace astro {
namespace graphics {

// --- Texture --------------------------------------
void clearTexture(Texture& texture, Color &color){
    std::fill(texture.data.begin(), texture.data.end(), color);
}
bool isInTextureBounds(Texture &texture, int x, int y){
    return x >= 0 && x < texture.width && y >= 0 && y < texture.height;
}
void putPixel(Texture& texture, int x, int y, Color &color){
    texture.data[texture.index(x, y)] = color;
}
const Color& getPixel(const Texture& texture, int x, int y) {
    return texture.data[texture.index(x, y)];
}

// Faster than std::floor
float fast_wrap(float val) { return val - static_cast<int>(val) + (val < 0); }
Color sampleTextureColor(const Texture& texture, Vec2f uv) {
    // Wrap UVs to 0.0 - 1.0
    float u = fast_wrap(uv.x);
    float v = 1.0f - fast_wrap(uv.y); // Flip V (Texture coordinates usually start top-left, UVs bottom-left)

    // Map to pixel dimensions
    int tx = static_cast<int>(u * texture.width);
    int ty = static_cast<int>(v * texture.height);

    // Safety clamp (can be removed if the texture is power of two)
    tx = (tx < 0) ? 0 : (tx >= texture.width)  ? texture.width - 1  : tx;
    ty = (ty < 0) ? 0 : (ty >= texture.height) ? texture.height - 1 : ty;
    return texture.data[ty * texture.width + tx]; // same as getPixel()
}
Vec4f sampleTexureColorAsVec4f(const Texture& texture, Vec2f uv) {
    return Vec4f(sampleTexureColorAsVec3f(texture, uv), 0.0f);
}
Vec3f sampleTexureColorAsVec3f(const Texture& texture, Vec2f uv) {
    static constexpr float inv255 = 1.0f / 255.0f;
    Color col = sampleTextureColor(texture, uv);
    return Vec3f(col[0] * inv255, col[1] * inv255, col[2] * inv255);
}
Vec4f sampleTexureVectorAsVec4f(const Texture& texture, Vec2f uv) {
    return Vec4f(sampleTexureColorAsVec3f(texture, uv), 0.0f);
}
Vec3f sampleTexureVectorAsVec3f(const Texture& texture, Vec2f uv) {
    static constexpr float inv255_times_two = 2.0f / 255.0f;
    Color col = sampleTextureColor(texture, uv);
    return Vec3f(
        col[0] * inv255_times_two - 1.0f,
        col[1] * inv255_times_two - 1.0f,
        col[2] * inv255_times_two - 1.0f
    );
}



// --- Z-Buffering ----------------------------------
void clearZBuffer(ZBuffer& zbuffer){
    std::fill(zbuffer.data.begin(), zbuffer.data.end(), zbuffer.FAR_VAL);
}
void putDepth(ZBuffer& zbuffer, int x, int y, double depth) {
    zbuffer.data[zbuffer.index(x, y)] = depth;
}
const double& getDepth(const ZBuffer& zbuffer, int x, int y) {
    return zbuffer.data[zbuffer.index(x, y)];
}
Texture zbuffer2Texture(const ZBuffer& zbuffer, Color col) {
    Texture texture(zbuffer.width, zbuffer.height);
    for(int i = 0; i < zbuffer.data.size(); i++) {
        double depth = zbuffer.data[i];
        if (depth < 0.0) depth = 0.0;
        if (depth > 1.0) depth = 1.0;
        double d_val = (1.0 - depth);
        texture.data[i] = {
           static_cast<uint8_t>(std::clamp((double)col.r * d_val, 0.0, 255.0)), 
           static_cast<uint8_t>(std::clamp((double)col.g * d_val, 0.0, 255.0)), 
           static_cast<uint8_t>(std::clamp((double)col.b * d_val, 0.0, 255.0)), 
            255};
    }
    return texture;
}



// --- 2D Rendering ---------------------------------
void draw2dLine(Texture& texture, int x1, int y1, int x2, int y2, Color &color){
    // Parametric implementation
    // t(x) = (x - x1) / (x2 - x1)
    // y(t) = y1 + (y2 - y1) * t

    float vx = x2 - x1;
    float vy = y2 - y1;

    // Transpose if it is more vertical than horizontal
    const bool transpose = std::abs(vy) > std::abs(vx);
    if (transpose){
        std::swap(x1, y1);
        std::swap(x2, y2);
        std::swap(vx, vy);
    }

    if(x1 > x2){
    // Make it left to rigth
        std::swap(x1, x2);
        std::swap(y1, y2);
    }
    
    if (vx == 0) return;
    for (int x = x1; x <= x2; x++){
        const float t = (x - x1) / (vx);
        const int y = std::round(y1 + vy * t);
        
        if (transpose) {
            if(isInTextureBounds(texture, y, x))
                putPixel(texture, y, x, color);
            continue;
        } 

        if(isInTextureBounds(texture, x, y))
            putPixel(texture, x, y, color);
    }

}
void draw2dLine(Texture& texture, Vec2i a, Vec2i b, Color &color) {
    draw2dLine(texture, a.x, a.y, b.x, b.y, color);
}
float signed_triangle_area(float x1, float y1, float x2, float y2, float x3, float y3){
    return .5*((y2-y1)*(x2+x1) + (y3-y2)*(x3+x2) + (y1-y3)*(x1+x3));
}
void draw2dTriangle(Texture &texture, int x1, int y1, int x2, int y2, int x3, int y3, Color color) {
    // Find bounding box
    int bbminx = std::max(0, (int)std::floor(std::min({x1, x2, x3})));
    int bbminy = std::max(0, (int)std::floor(std::min({y1, y2, y3})));
    int bbmaxx = std::min(texture.width - 1, (int)std::ceil(std::max({x1, x2, x3})));
    int bbmaxy = std::min(texture.height - 1, (int)std::ceil(std::max({y1, y2, y3})));

    double total_area = signed_triangle_area(x1, y1, x2, y2, x3, y3);

    #pragma omp parallel for
    for (int x=bbminx; x<=bbmaxx; x++) {
        for (int y=bbminy; y<=bbmaxy; y++) {
            float alpha = signed_triangle_area(x, y, x2, y2, x3, y3) / total_area;
            float beta  = signed_triangle_area(x, y, x3, y3, x1, y1) / total_area;
            float gamma = signed_triangle_area(x, y, x1, y1, x2, y2) / total_area;
            if (alpha<0 || beta<0 || gamma<0) continue; // negative barycentric coordinate => the pixel is outside the triangle
            
            putPixel(texture, x, y,color);
        }
    }

}
void draw2dTriangle(Texture& texture, Vec2i a, Vec2i b, Vec2i c, Color color) {
    draw2dTriangle(texture, a.x, a.y, b.x, b.y, c.x, c.y, color);
}



// --- 3D Rendering ---------------------------------
// Basic Shader
void BasicShader::updateMVP() {
    MV = viewMatrix * modelMatrix;
    MVP = projectionMatrix * MV;
    MV_invT = transpose(inverse(MV));
}
bool BasicShader::vertex(const VertexAttributes& in_vert, Varyings& out_varying) const {
    out_varying.pos = MVP * in_vert.pos;
    out_varying.worldPos = MV * in_vert.pos;
    out_varying.normal = (MV_invT * Vec4f(in_vert.normal, 0.0)).xyz;
    out_varying.tangent = (MV_invT * Vec4f(in_vert.tangent, 0.0)).xyz;
    out_varying.uv = in_vert.uv;
    return true;
}
bool BasicShader::fragment(const Varyings& interpolated, Color& out_color) const {
    out_color = Color(255, 255, 255, 255);
    return true;
}

// Phong Shader
Vec4f PhongShader::calculatePhong(const Light& light, const Vec3f& normal, const Vec3f& worldPos, const Vec3f& cameraPos, const Vec4f& specMask) const {
    Vec3f L; // Light Direction
    float attenuation = 1.0f; // Light Strength

    // Point light
    if( light.type == Light::POINT) {
        L = light.worldPos - worldPos;
        float dist = len(L); 
        if(dist > light.range) return Vec4f(0.0f); // No light

        attenuation = light.intensity / (1.0f + dist*dist); // to prevent divission by 0
        L = normalize(L);
    }

    // Directional light
    else {
        L = normalize(light.worldDir * -1.0f);
        attenuation = light.intensity;
    }
    
    // Diffuse (lambertian)
    float nDotL = dot(normal, L);
    float diff = std::max(0.0f,nDotL); // Lambertian reflection (N dot L)
    Vec4f diffusePart = light.color * diff * material->diffuseCoeff;

    // Specular (phong)
    Vec4f specularPart(0.0f);
    if(nDotL > 0.0f) {
        Vec3f V = normalize(cameraPos - worldPos);
        Vec3f R = normalize((normal * (2.0f * dot(normal, L))) - L); // Reflect -L around normal
        float spec = std::pow(std::max(dot(V, R), 0.0f), material->shininess);
        specularPart = light.color * (spec * material->specularCoeff) * specMask;
    }
    return (diffusePart + specularPart) * attenuation;
}
bool PhongShader::fragment(const Varyings& interpolated, Color& out_color) const {
    Vec3f worldPos = interpolated.worldPos.xyz;
    Vec3f V = normalize(cameraPos - worldPos);
    
    // Normal Mapping
    Vec3f finalNormal = normalize(interpolated.normal); 
    if (material->normalMap) {
        Vec3f T = normalize(interpolated.tangent);
        // Gram-Schmidt is usually done at load time now, but T = normalize(T - N * dot(T, N)) 
        // if you need it here.
        Vec3f B = cross(finalNormal, T);
        Vec3f mappedNormal = sampleTexureVectorAsVec3f(*material->normalMap, interpolated.uv);
        finalNormal = normalize(T * mappedNormal.x + B * mappedNormal.y + finalNormal * mappedNormal.z);
    }

    // Texture samplers (avoid Vec4f at sampling if alpha is not needed)
    Vec3f texColor = (material->colorTexture) ? sampleTexureColorAsVec3f(*material->colorTexture, interpolated.uv) : material->color.xyz;
    float specMask = (material->specularMap) ? sampleTexureColorAsVec3f(*material->specularMap, interpolated.uv).x : 1.0f;
    Vec3f emissive = (material->glowMap) ? sampleTexureColorAsVec3f(*material->glowMap, interpolated.uv) : Vec3f(0.0f);

    // Compute accumulated light
    Vec3f accumulatedLight(0.0f);
    for(const Light& light : sceneLights) {
        Vec3f L;
        float attenuation = light.intensity;

        if (light.type == Light::POINT) {
            L = light.worldPos - worldPos;
            float distSq = dot(L, L); // Use squared length to avoid sqrt if possible
            if (distSq > (light.range * light.range)) continue;
            float dist = std::sqrt(distSq);
            attenuation /= (1.0f + distSq); 
            L = L / dist; // faster than normalize(L)
        } else {
            L = normalize(light.worldDir * -1.0f);
        }

        // Diffuse
        float nDotL = dot(finalNormal, L);
        if (nDotL <= 0.0f) continue; // light is behind surface

        accumulatedLight = accumulatedLight + light.color.xyz * (nDotL * material->diffuseCoeff * attenuation);

        // Specular (Blinn-Phong)
        Vec3f H = normalize(L + V); // Halfway vector
        float nDotH = std::max(0.0f, dot(finalNormal, H));
        
        // Fast Power approximation (or use a smaller lookup table)
        float spec = std::pow(nDotH, material->shininess); 
        accumulatedLight = accumulatedLight + light.color.xyz * (spec * material->specularCoeff * specMask * attenuation);
    }
    
    // Final composite (Doing math in float, then converting to uint8 at the very end)
    Vec3f combined = (texColor * accumulatedLight) + emissive;
    
    out_color = Color(
        (uint8_t)(std::min(combined.x, 1.0f) * 255.0f),
        (uint8_t)(std::min(combined.y, 1.0f) * 255.0f),
        (uint8_t)(std::min(combined.z, 1.0f) * 255.0f),
        (uint8_t)(material->oppacity * 255.0f)
    );
    return true;
}

// Renderer
/**
 * @brief Inverse multiplied components for fast computation
 */
struct PreppedVarying {
    Vec2f uvw;
    Vec3f normalw;
    Vec3f tangentw;
    Vec3f worldPosw;
};

void TDRenderer::renderTriangle(Texture& texture, ZBuffer& zbuffer, const Triangle& triangle, const IShader& shader) {
    std::array<Varyings, 3> varyings{};
    std::array<Vec3f, 3> screen_pts{};
    std::array<float, 3> inv_w{};

    // Vertex Shader
    for (int i = 0; i < 3; ++i) {
        if (!shader.vertex(triangle[i], varyings[i])) return;
        if (varyings[i].pos.w < 0.1f) return;


        inv_w[i] = 1.0f / varyings[i].pos.w;
        screen_pts[i] = {
            (varyings[i].pos.x * inv_w[i] + 1.0f) * 0.5f * (float)texture.width,
            (1.0f - varyings[i].pos.y * inv_w[i]) * 0.5f * (float)texture.height,
            varyings[i].pos.z * inv_w[i] 
        };
    }

    // Pre-multiplied attibutes
    std::array<PreppedVarying, 3> pv;
    for (int i = 0; i < 3; ++i) {
        pv[i].uvw = varyings[i].uv * inv_w[i];
        pv[i].normalw = varyings[i].normal * inv_w[i];
        pv[i].tangentw = varyings[i].tangent * inv_w[i];
        pv[i].worldPosw = varyings[i].worldPos.xyz * inv_w[i];
    }

    // Backface Culling
    double total_area = signed_triangle_area(screen_pts[0].x, screen_pts[0].y, 
                                           screen_pts[1].x, screen_pts[1].y, 
                                           screen_pts[2].x, screen_pts[2].y);
    if (total_area > -0.0001) return; 

    // Rasterization Setup
    int bbminx = std::max(0, (int)std::floor(std::min({screen_pts[0].x, screen_pts[1].x, screen_pts[2].x})));
    int bbminy = std::max(0, (int)std::floor(std::min({screen_pts[0].y, screen_pts[1].y, screen_pts[2].y})));
    int bbmaxx = std::min(texture.width - 1, (int)std::ceil(std::max({screen_pts[0].x, screen_pts[1].x, screen_pts[2].x})));
    int bbmaxy = std::min(texture.height - 1, (int)std::ceil(std::max({screen_pts[0].y, screen_pts[1].y, screen_pts[2].y})));
    float inv_total_area = 1.0f / (float)total_area;

    // Rasterization loop
    for (int y = bbminy; y <= bbmaxy; ++y) {
        for (int x = bbminx; x <= bbmaxx; ++x) {
            float alpha = signed_triangle_area(x, y, screen_pts[1].x, screen_pts[1].y, screen_pts[2].x, screen_pts[2].y) * inv_total_area;
            float beta  = signed_triangle_area(x, y, screen_pts[2].x, screen_pts[2].y, screen_pts[0].x, screen_pts[0].y) * inv_total_area;
            float gamma = 1.0f - alpha - beta;

            if (alpha < 0 || beta < 0 || gamma < 0) continue;

            float depth = alpha * screen_pts[0].z + beta * screen_pts[1].z + gamma * screen_pts[2].z;
            if (depth >= getDepth(zbuffer, x, y)) continue;

            // 3. Perspective Reconstruction
            // First, interpolate the reciprocal w
            float interpolated_inv_w = alpha * inv_w[0] + beta * inv_w[1] + gamma * inv_w[2];
            float w = 1.0f / interpolated_inv_w; // The only division needed!

            // Linearly interpolate the pre-multiplied values and multiply by w to recover
            Varyings interp;
            interp.uv       = (pv[0].uvw * alpha + pv[1].uvw * beta + pv[2].uvw * gamma) * w;
            interp.normal   = (pv[0].normalw * alpha + pv[1].normalw * beta + pv[2].normalw * gamma) * w;
            interp.tangent  = (pv[0].tangentw * alpha + pv[1].tangentw * beta + pv[2].tangentw * gamma) * w;
            interp.worldPos = Vec4f((pv[0].worldPosw * alpha + pv[1].worldPosw * beta + pv[2].worldPosw * gamma) * w, 1.0f);

            Color fragColor;
            if (shader.fragment(interp, fragColor)) {
                putDepth(zbuffer, x, y, depth);
                putPixel(texture, x, y, fragColor);
            }
        }
    }

}

}
}