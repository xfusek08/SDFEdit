
#include <SceneLoader.h>
#include <data/primitives.h>

#include <RenderBase/asserts.h>
#include <RenderBase/tools/utils.h>

#include <glm/gtc/type_ptr.hpp>

#include <nlohmann/json.hpp>

#include <unordered_map>
#include <fstream>

using namespace std;
using Json = nlohmann::json;

static const std::unordered_map<string, PrimitiveType> primitiveTypeDict = {
    { "Sphere",    PrimitiveType::ptSphere },
    { "Capsule",   PrimitiveType::ptCapsule },
    { "Torus",     PrimitiveType::ptTorus },
    { "Box",       PrimitiveType::ptBox },
    { "Cylinder",  PrimitiveType::ptCylinder },
    { "Cone",      PrimitiveType::ptCone },
    { "RoundCone", PrimitiveType::ptRoundCone },
};

static const std::unordered_map<std::string, GeometryOperation> operationDict = {
    { "Add",       GeometryOperation::opAdd },
    { "Subtract",  GeometryOperation::opSubtract },
    { "Sub",       GeometryOperation::opSubtract },
    { "Intersect", GeometryOperation::opIntersect },
    { "Paint",     GeometryOperation::opPaint },
};

template<size_t L>
glm::vec<L, float> jsonToVec(Json value)
{
    auto res = glm::vec<L, float>(0);
    int index = 0;
    for (auto& val : value) {
        res[index++] = val.get<float>();
        if (index >= L) break;
    }
    return res;
}

template<typename T>
inline T getScalar(Json json, string ident, T defaultValue)
{
    if (json.contains(ident)) {
        return json[ident].get<T>();
    } else {
        return defaultValue;
    }
}

inline float32 getFloat(Json json, string ident, float32 defaultValue = 0.0)
{
    return getScalar<float32>(json, ident, defaultValue);
}

inline glm::vec3 getVec3(Json json, string ident, glm::vec3 defaultValue = glm::vec3(0))
{
    if (json.contains(ident)) {
        return jsonToVec<3>(json[ident]);
    } else {
        return defaultValue;
    }
}

inline Transform getTransform(Json json, Transform defaultValue = Transform())
{
    return Transform(
        getVec3(json, "position"),
        glm::radians(getVec3(json, "rotation"))
    );
}

inline string getString(Json json, string ident, string defaultValue = "")
{
    return getScalar<string>(json, ident, defaultValue);
}

shared_ptr<Scene> prepareShaderSceneData(string fileName)
{
    // load json
    Json json;
    std::ifstream stream(fileName);
    if (stream.good()) {
        stream >> json;
    } else {
        json = Json::parse(fileName);
    }
    
    unordered_map<string, shared_ptr<Geometry>> geometryCache = {};
    auto getGeometryByName = [&] (string geometryName) {
        shared_ptr<Geometry> geometry = geometryCache[geometryName];
        if (geometry == nullptr && json["geometries"].contains(geometryName)) {
            geometry = make_shared<Geometry>();
            for (auto& editJson : json["geometries"][geometryName]) {
                if (editJson.contains("type"))  {
                    
                    // common properties
                    auto operation = rb::utils::getOr(operationDict, getString(editJson, "op"), 0);
                    auto blending  = getFloat(editJson, "blending");
                    auto transform = getTransform(editJson);
                    
                    // specific edit constructors
                    switch(rb::utils::getOr(primitiveTypeDict, editJson["type"].get<string>(), -1)) {
                        case PrimitiveType::ptSphere: geometry->addEdit(primitives::Sphere::createEdit(
                            operation,
                            transform,
                            getFloat(editJson, "radius", 1.0),
                            rb::colors::white,
                            blending
                        )); break;
                        case PrimitiveType::ptCapsule: geometry->addEdit(primitives::Capsule::createEdit(
                            operation,
                            transform,
                            getFloat(editJson, "width",  0.25),
                            getFloat(editJson, "height", 1.0),
                            getFloat(editJson, "radius", 0.25),
                            rb::colors::white,
                            blending
                        )); break;
                        case PrimitiveType::ptTorus: geometry->addEdit(primitives::Torus::createEdit(
                            operation,
                            transform,
                            getFloat(editJson, "outerRadius", 2.5),
                            getFloat(editJson, "innerRadius", 0.25),
                            rb::colors::white,
                            blending
                        )); break;
                        case PrimitiveType::ptBox: geometry->addEdit(primitives::Box::createEdit(
                            operation,
                            transform,
                            getVec3(editJson, "dimensions", {1, 1, 1}),
                            getFloat(editJson, "rounding"),
                            rb::colors::white,
                            blending
                        )); break;
                        case PrimitiveType::ptCylinder: geometry->addEdit(primitives::Cylinder::createEdit(
                            operation,
                            transform,
                            getFloat(editJson, "height",   1.0),
                            getFloat(editJson, "radius",   0.5),
                            getFloat(editJson, "rounding", 0.0),
                            rb::colors::white,
                            blending
                        )); break;
                        case PrimitiveType::ptCone: geometry->addEdit(primitives::Cone::createEdit(
                            operation,
                            transform,
                            getFloat(editJson, "radiusTop",    0),
                            getFloat(editJson, "radiusBottom", 1.5),
                            getFloat(editJson, "height",       3.0),
                            getFloat(editJson, "rounding",     0.0),
                            rb::colors::white,
                            blending
                        )); break;
                        case PrimitiveType::ptRoundCone: geometry->addEdit(primitives::RoundCone::createEdit(
                            operation,
                            transform,
                            getFloat(editJson, "radiusTop",    0.125),
                            getFloat(editJson, "radiusBottom", 0.25),
                            getFloat(editJson, "height",       1),
                            rb::colors::white,
                            blending
                        )); break;
                    }
                }
            }
        }
        geometryCache[geometryName] = geometry;
        return geometry;
    };
    
    unordered_map<string, Material> materialCache = {};
    auto getMaterialByName = [&](string materialName) {
        if (materialCache.find(materialName) != materialCache.end()) {
            return materialCache[materialName];
        }
        
        Material res = {};
        if (json["materials"].contains(materialName)) {
            auto materialJson = json["materials"][materialName];
            res.color = getVec3(materialJson, "color");
            res.shininess = getFloat(materialJson, "shininess");
        }
        
        materialCache[materialName] = res;
        return res;
    };
    
    auto scene = make_shared<Scene>();
    scene->models.reserve(json["models"].size());
    for (auto& [key, value] : json["models"].items()) {
        if (value.contains("geometry")) {
            auto model = Model(getGeometryByName(value["geometry"].get<string>()));
            model.transform = getTransform(value);
            if (value.contains("material")) {
                model.material = getMaterialByName(value["material"].get<string>());
            }
            scene->models.push_back(model);
        }
    }
    return scene;
}
