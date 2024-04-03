#include "ai.h"
#include <string.h>

AI_SHADER_NODE_EXPORT_METHODS(SimpleMethods);

enum LightRayParams
{
    p_Normal,
    p_Power,
    p_ClampLo,
    p_ClampHi,
    p_Mult
};

node_parameters
{
    AiParameterVec("Normal", 0.0, 1.0, 0.0);
    AiParameterFlt("Power", 100.0);
    AiParameterFlt("ClampLo", 0.0);
    AiParameterFlt("ClampHi", 10.0);
    AiParameterFlt("Mult", 28.0);
}

node_initialize
{
}

node_update
{
}

node_finish
{
}


static float spec(AtShaderGlobals* sg, AtVector N, float power, float clampLo, float clampHi, float mult)
{
    AiLightsPrepare(sg);

    AtLightSample light_sample;
    float result = 0.0;
    while (AiLightsGetSample(sg, light_sample))
    {

        //AtVector reflectDir = AiReflect(light_sample.Ld, N);
        AtVector halfwayDir = AiV3Normalize(light_sample.Ld + sg->Rd);
        float spec = AiMax(AiV3Dot(N, halfwayDir), 0.0f);
        spec = AiFastPow(spec, power);
        spec = AiClamp(spec, clampLo, clampHi);
        spec *= mult;
        result += spec;
    }
    return result;
}

shader_evaluate
{
    AtVector inNorm = AiShaderEvalParamVec(p_Normal);
    float pow = AiShaderEvalParamFlt(p_Power);
    float clampLo = AiShaderEvalParamFlt(p_ClampLo);
    float clampHi = AiShaderEvalParamFlt(p_ClampHi);
    float mult = AiShaderEvalParamFlt(p_Mult);

    float evalSpec = spec(sg, inNorm, pow, clampLo, clampHi, mult);


    AtRGB emission = AtRGB(evalSpec, evalSpec, evalSpec);

    sg->out.RGB() = emission;

}

node_loader
{
        if (i > 0)
        return false;
        node->methods = SimpleMethods;
        node->output_type = AI_TYPE_RGB;
        node->name = "nyx_blinn";
        node->node_type = AI_NODE_SHADER;
        strcpy_s(node->version, AI_VERSION);
        return true;
}