//
//  VROMaterial.h
//  ViroRenderer
//
//  Created by Raj Advani on 11/17/15.
//  Copyright © 2015 Viro Media. All rights reserved.
//

#ifndef VROMaterial_h
#define VROMaterial_h

#include <memory>
#include "VROMaterialVisual.h"
#include "VROAnimatable.h"

enum class VROCullMode {
    Back,
    Front,
    None
};

enum class VROBlendMode {
    Alpha,
    Add,
    Subtract,
    Multiply,
    Screen,
    Replace
};

enum class VROTransparencyMode {
    AOne,
    RGBZero
};

enum class VROLightingModel {
    Phong,
    Blinn,
    Lambert,
    Constant
};

class VROLight;
class VRODriver;
class VROSortKey;
class VROMaterialSubstrate;
class VROShaderModifier;

/*
 Manages the lighting and shading attributes associated with the surface of a geometry that
 define its appearance when rendered. When you create a material, you define a collection of
 visual attributes and their options, which you can then reuse for multiple geometries 
 in a scene.
 */
class VROMaterial : public VROAnimatable {
    
public:
    
    VROMaterial();
    virtual ~VROMaterial();
    
    /*
     Copy constructor for this material. Texture contents use shared references.
     */
    VROMaterial(std::shared_ptr<VROMaterial> material);
    
    /*
     Set a name for this material. No functionality, only used for 
     debugging.
     */
    void setName(std::string name) {
        _name = name;
    }
    std::string getName() const {
        return _name;
    }
    
    uint32_t getMaterialId() const {
        return _materialId;
    }
    
    void bindShader(VRODriver &driver);
    void bindLights(int lightsHash,
                    const std::vector<std::shared_ptr<VROLight>> &lights,
                    const VRORenderContext &context,
                    VRODriver &driver);
    
    VROMaterialVisual &getDiffuse() const {
        return *_diffuse;
    }
    VROMaterialVisual &getSpecular() const {
        return *_specular;
    }
    VROMaterialVisual &getNormal() const {
        return *_normal;
    }
    VROMaterialVisual &getReflective() const {
        return *_reflective;
    }
    VROMaterialVisual &getEmission() const {
        return *_emission;
    }
    VROMaterialVisual &getTransparent() const {
        return *_transparent;
    }
    VROMaterialVisual &getMultiply() const {
        return *_multiply;
    }
    VROMaterialVisual &getAmbientOcclusion() const {
        return *_ambientOcclusion;
    }
    VROMaterialVisual &getSelfIllumination() const {
        return *_selfIllumination;
    }
    
    void setShininess(float shininess);
    float getShininess() const {
        return _shininess;
    }
    
    void setFresnelExponent(float fresnelExponent);
    float getFresnelExponent() const {
        return _fresnelExponent;
    }
    
    void setTransparency(float transparency);
    float getTransparency() const {
        return _transparency;
    }
    
    void setTransparencyMode(VROTransparencyMode mode) {
        _transparencyMode = mode;
    }
    VROTransparencyMode getTransparencyMode() const {
        return _transparencyMode;
    }
    
    void setLightingModel(VROLightingModel model) {
        _lightingModel = model;
    }
    VROLightingModel getLightingModel() const {
        return _lightingModel;
    }
    
    void setCullMode(VROCullMode cullMode) {
        _cullMode = cullMode;
    }
    VROCullMode getCullMode() const {
        return _cullMode;
    }
    
    bool isLitPerPixel() const {
        return _litPerPixel;
    }
    
    VROBlendMode getBlendMode() const {
        return _blendMode;
    }
    bool getWritesToDepthBuffer() const {
        return _writesToDepthBuffer;
    }
    bool getReadsFromDepthBuffer() const {
        return _readsFromDepthBuffer;
    }
    
    void setWritesToDepthBuffer(bool writesToDepthBuffer) {
        _writesToDepthBuffer = writesToDepthBuffer;
        updateSubstrate();
    }
    void setReadsFromDepthBuffer(bool readsFromDepthBuffer) {
        _readsFromDepthBuffer = readsFromDepthBuffer;
        updateSubstrate();
    }

    void addShaderModifier(std::shared_ptr<VROShaderModifier> modifier);
    const std::vector<std::shared_ptr<VROShaderModifier>> &getShaderModifiers() const {
        return _shaderModifiers;
    }
    
    /*
     Make a snapshot of this material and cross-fade that snapshot out,
     bringing in the current material. Used to animate material changes.
     No effect if there is no active animation transaction.
     */
    void fadeSnapshot();
    std::shared_ptr<VROMaterial> getOutgoing() const {
        return _outgoing;
    }
    
    /*
     Check if the material has been updated since the last substrate was
     created.
     */
    bool isUpdated() {
        return _substrate == nullptr;
    }
    
    /*
     Force the substrate of this material to update on the next render cycle.
     */
    void updateSubstrate();
    
    /*
     Get the representation of this material in the underlying graphics 
     technology.
     */
    VROMaterialSubstrate *const getSubstrate(VRODriver &driver);
    
    /*
     Update the given sort key with fields from this material.
     */
    void updateSortKey(VROSortKey &key, VRODriver &driver);
    
private:
    
    uint32_t _materialId;
    
    /*
     The visual properties associated with the material.
     */
    VROMaterialVisual *_diffuse;
    VROMaterialVisual *_specular;
    VROMaterialVisual *_normal;
    VROMaterialVisual *_reflective;
    VROMaterialVisual *_emission;
    VROMaterialVisual *_transparent;
    VROMaterialVisual *_multiply;
    VROMaterialVisual *_ambientOcclusion;
    VROMaterialVisual *_selfIllumination;
    
    /*
     User-provided name of the material.
     */
    std::string _name;
    
    /*
     The sharpness of specular highlights.
     */
    float _shininess;
    
    /*
     Factor affecting material reflectivity.
     */
    float _fresnelExponent;
    
    /*
     Uniform transparency of the material.
     */
    float _transparency;
    
    /*
     The mode used to calculate transparency.
     */
    VROTransparencyMode _transparencyMode;
    
    /*
     The lighting model to use to compute the interaction between
     the lights in the scene and this material's visual properties.
     */
    VROLightingModel _lightingModel;
    
    /*
     True means use per-pixel lighting, false means use per-vertex lighting.
     */
    bool _litPerPixel;
    
    /*
     Set to cull back faces, front faces, or none.
     */
    VROCullMode _cullMode;
    
    /*
     Determines how pixel colors rendered using this material blend with 
     pixel colors already in the render target.
     */
    VROBlendMode _blendMode;
    
    /*
     Depth write and read settings.
     */
    bool _writesToDepthBuffer, _readsFromDepthBuffer;
    
    /*
     Version of this material that's being animated away. Populated with the current
     values of this material whenever this material is changed.
     */
    std::shared_ptr<VROMaterial> _outgoing;
    
    /*
     Modifiers to alter the shader code.
     */
    std::vector<std::shared_ptr<VROShaderModifier>> _shaderModifiers;
    
    /*
     Representation of this material in the underlying graphics hardware.
     */
    VROMaterialSubstrate *_substrate;
    
    void removeOutgoingMaterial();
    
};

#endif /* VROMaterial_h */