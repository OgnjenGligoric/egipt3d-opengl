#pragma once
#ifndef MODEL_RENDERER_H
#define MODEL_RENDERER_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "model.h"
#include "shader.h"


class ModelRenderer
{
public:
	glm::vec3 Position;
	glm::vec3 Size;
	glm::vec3 Rotation;
    float Alpha;
    float ApplyLight;
	Shader shader;

	ModelRenderer(Shader& shader, glm::vec3 position, glm::vec3 size = glm::vec3(10.0f, 10.0f, 10.0f),
	              glm::vec3 rotation = glm::vec3(0.0f, 0.0f, 0.0f), float alpha = 1.0f) :
		Position(position),
		Size(size),
		Rotation(rotation),
		shader(shader),
		Alpha(alpha),
		ApplyLight(1.0){}
	ModelRenderer() {}
	~ModelRenderer();
	void DrawModel(Model& model);
private:
};

#endif
void ModelRenderer::DrawModel(Model& model)
{
	glBindTexture(GL_TEXTURE_2D, 0);
    glm::vec3 pointLightPositions[] = {
        glm::vec3(601.0f, 440.0f, -596.0f),
        glm::vec3(500.0f, 330.0f, 150.0f),
        glm::vec3(-200.0f, 250.0f, 350.0f),
        glm::vec3(0.0f,  300.0f, -3.0f)
    };
	glm::mat4 model_shader = glm::mat4(1.0f);
	model_shader = glm::translate(model_shader, Position); // translate it down so it's at the center of the scene
	model_shader = glm::scale(model_shader, Size);
	model_shader = glm::rotate(model_shader, glm::radians(Rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
	model_shader = glm::rotate(model_shader, glm::radians(Rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
	model_shader = glm::rotate(model_shader, glm::radians(Rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
	shader.Use().SetMatrix4("model", model_shader);
    shader.SetFloat("alpha", Alpha);
    shader.SetFloat("applyLight", ApplyLight);
	shader.SetVector3f("lightColor", 1.0f, 1.0f, 1.0f);
    //shader.SetVector3f("dirLight.direction", 0.0f, -1.0f, -1.0f);
    shader.SetVector3f("dirLight.ambient", 0.15f, 0.15f, 0.15f);
    shader.SetVector3f("dirLight.diffuse", 1.0f, 1.0f, 1.0f);
    shader.SetVector3f("dirLight.specular", 0.2f, 0.2f, 0.2f);
    //// point light 1
    shader.SetVector3f("pointLights[0].position", pointLightPositions[0]);
    shader.SetVector3f("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
    shader.SetVector3f("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
    shader.SetVector3f("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
    shader.SetFloat("pointLights[0].constant", 1.0f);
    shader.SetFloat("pointLights[0].linear", 0.0014f);
    shader.SetFloat("pointLights[0].quadratic", 0.000007f);
    //// point light 2
    shader.SetVector3f("pointLights[1].position", pointLightPositions[1]);
    shader.SetVector3f("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
    shader.SetVector3f("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
    shader.SetVector3f("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
    shader.SetFloat("pointLights[1].constant", 1.0f);
    shader.SetFloat("pointLights[1].linear", 0.0014f);
    shader.SetFloat("pointLights[1].quadratic", 0.000007f);
    //// point light 3
    shader.SetVector3f("pointLights[2].position", pointLightPositions[2]);
    shader.SetVector3f("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
    shader.SetVector3f("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
    shader.SetVector3f("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
    shader.SetFloat("pointLights[2].constant", 1.0f);
    shader.SetFloat("pointLights[2].linear", 0.0014f);
    shader.SetFloat("pointLights[2].quadratic", 0.000007f);
    //// point light 4
    //shader.SetVector3f("pointLights[3].position", pointLightPositions[3]);
    //shader.SetVector3f("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
    //shader.SetVector3f("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
    //shader.SetVector3f("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
    //shader.SetFloat("pointLights[3].constant", 1.0f);
    //shader.SetFloat("pointLights[3].linear", 0.09f);
    //shader.SetFloat("pointLights[3].quadratic", 0.032f);
    //// spotLight
    shader.SetVector3f("spotLight.position", 601.0f, 440.0f, -596.0f);
    //shader.SetVector3f("spotLight.direction", -1.0f, -0.5f, 0.0f);
    shader.SetVector3f("spotLight.ambient", 0.0f, 0.0f, 0.0f);
    shader.SetVector3f("spotLight.diffuse", 1.0f, 0.0f, 0.0f);
    shader.SetVector3f("spotLight.specular", 1.0f, 1.0f, 1.0f);
    shader.SetFloat("spotLight.constant", 1.0f);
    shader.SetFloat("spotLight.linear", 0.00014);
    shader.SetFloat("spotLight.quadratic", 0.00000007);
    shader.SetFloat("spotLight.cutOff", glm::cos(glm::radians(5.0f)));
    shader.SetFloat("spotLight.outerCutOff", glm::cos(glm::radians(7.5f)));

	model.Draw(shader);
}

ModelRenderer::~ModelRenderer()
{
}