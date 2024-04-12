// TODO
// point shadows - just copy the code?

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <learnopengl/filesystem.h>
#include <learnopengl/shader.h>
#include <learnopengl/camera.h>
#include <learnopengl/model.h>

#include <iostream>

void framebuffer_size_callback(__attribute__((unused)) GLFWwindow *window, int width, int height);

void processInput(GLFWwindow *window);

void mouse_callback(__attribute__((unused)) GLFWwindow *window, double xPos, double yPos);

void
scroll_callback(__attribute__((unused)) GLFWwindow *window, __attribute__((unused)) double xOffset, double yOffset);

unsigned int loadCubemap(vector<std::string> faces);

unsigned int loadTexture(char const *path);


// settings
const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;


int main() {
    // window init
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "project", nullptr, nullptr);
    if (window == nullptr) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // cursor settings
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

//    stbi_set_flip_vertically_on_load(true);

    glEnable(GL_DEPTH_TEST);

    // wireframe mode
//    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // -----------------------------------------------------------------------------------------------------------


    // football field
    float fieldVertices[] = {
            // positions                // normals                      // texture coords
            0.5f, 0.8f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, // top right
            0.5f, -0.8f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom right
            -0.5f, -0.8f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, // bottom left
            -0.5f, 0.8f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f  // top left

    };

    unsigned int fieldIndices[] = {
            0, 1, 3, // first triangle
            1, 2, 3  // second triangle
    };

    unsigned int fieldVBO, fieldVAO, fieldEBO;
    glGenVertexArrays(1, &fieldVAO);
    glGenBuffers(1, &fieldVBO);
    glGenBuffers(1, &fieldEBO);
    glBindVertexArray(fieldVAO);
    glBindBuffer(GL_ARRAY_BUFFER, fieldVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(fieldVertices), fieldVertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, fieldEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(fieldIndices), fieldIndices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);

    Shader fieldShader("resources/shaders/field.vs", "resources/shaders/field.fs");

    unsigned int fieldDiffuseMap = loadTexture(FileSystem::getPath("resources/textures/field.jpeg").c_str());
    fieldShader.setInt("material.diffuse", 0);
    fieldShader.setFloat("material.shininess", 1.0f);

    // skybox
    float skyboxVertices[] = {
            // positions
            -1.0f, 1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, 1.0f, -1.0f,
            -1.0f, 1.0f, -1.0f,

            -1.0f, -1.0f, 1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f, 1.0f, -1.0f,
            -1.0f, 1.0f, -1.0f,
            -1.0f, 1.0f, 1.0f,
            -1.0f, -1.0f, 1.0f,

            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, 1.0f,
            1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,

            -1.0f, -1.0f, 1.0f,
            -1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f,
            1.0f, -1.0f, 1.0f,
            -1.0f, -1.0f, 1.0f,

            -1.0f, 1.0f, -1.0f,
            1.0f, 1.0f, -1.0f,
            1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f,
            -1.0f, 1.0f, 1.0f,
            -1.0f, 1.0f, -1.0f,

            -1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f, 1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f, 1.0f,
            1.0f, -1.0f, 1.0f
    };

    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) 0);
    glBindVertexArray(0);

    vector<std::string> skyboxFaces
            {
                    "resources/textures/football-skybox/posx.jpg", // right
                    "resources/textures/football-skybox/negx.jpg", // left
                    "resources/textures/football-skybox/posy.jpg", // top
                    "resources/textures/football-skybox/negy.jpg", // bottom
                    "resources/textures/football-skybox/posz.jpg", // front
                    "resources/textures/football-skybox/negz.jpg"  // back
            };

    unsigned int cubemapTexture = loadCubemap(skyboxFaces);

    Shader skyboxShader("resources/shaders/skybox.vs", "resources/shaders/skybox.fs");
    skyboxShader.use();
    skyboxShader.setInt("skybox", 0);

    // clouds
    float cloudVertices[] = {
            // positions                  // texture coords
            1.0f, 1.0f, 0.0f, 1.0f, 0.0f, // top right
            1.0f, -1.0f, 0.0f, 0.0f, 0.0f, // bottom right
            -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, // bottom left
            -1.0f, 1.0f, 0.0f, 1.0f, 1.0f  // top left
    };

    unsigned int cloudIndices[] = {
            0, 1, 3,   // first triangle
            1, 2, 3    // second triangle
    };

    vector<glm::vec3> clouds // positions
            {
                    glm::vec3(-6.5f, 5.0f, -0.48f),
                    glm::vec3(2.5f, 5.0f, 1.51f),
                    glm::vec3(10.0f, 5.0f, 4.7f),
                    glm::vec3(-15.3f, 5.0f, -5.3f),
                    glm::vec3(1.5f, 5.0f, -8.6f)
            };

    unsigned int cloudVAO, cloudVBO, cloudEBO;
    glGenVertexArrays(1, &cloudVAO);
    glGenBuffers(1, &cloudVBO);
    glGenBuffers(1, &cloudEBO);
    glBindVertexArray(cloudVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cloudVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cloudVertices), cloudVertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cloudEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cloudIndices), cloudIndices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) (3 * sizeof(float)));
    glBindVertexArray(0);

    unsigned int cloudTexture = loadTexture(FileSystem::getPath("resources/textures/cloud.png").c_str());

    Shader cloudShader("resources/shaders/cloud.vs", "resources/shaders/cloud.fs");
    cloudShader.use();
    cloudShader.setInt("texture1", 0);

    // lamp
    Shader lampShader("resources/shaders/lamp.vs", "resources/shaders/lamp.fs");
    Model lampModel(FileSystem::getPath("resources/objects/lamp/Lamp05b.obj"));
    lampModel.SetShaderTextureNamePrefix("material.");

    // football
    Shader footballShader("resources/shaders/football.vs", "resources/shaders/football.fs");
    Model footballModel(FileSystem::getPath("resources/objects/football/football.obj"));
    footballModel.SetShaderTextureNamePrefix("material.");

    // render loop
    while (!glfwWindowShouldClose(window)) {

        // per-frame time logic
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(0.0f, 0.2f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // declaring matrices and vectors
        glm::mat4 projection;
        glm::mat4 view;
        glm::mat4 model;
        glm::vec3 dirLightDirection(-0.3f, -1.0f, 0.0f);
        glm::vec3 dirLightAmbient(0.05f, 0.05f, 0.05f);
        glm::vec3 dirLightDiffuse(0.2f, 0.2f, 0.2f);
        glm::vec3 dirLightSpecular(1.0f, 1.0f, 1.0f);
        glm::vec3 pointLightPosition(10.0f, 5.0f, 0.0f);
        glm::vec3 pointLightAmbient(0.3f, 0.3f, 0.3f);
        glm::vec3 pointLightDiffuse(0.95f, 0.95f, 0.95f);
        glm::vec3 pointLightSpecular(1.0f, 1.0f, 1.0f);

        // projection and view matrix stay the same
        projection = glm::perspective(glm::radians(camera.Zoom), (float) SCR_WIDTH / (float) SCR_HEIGHT, 0.1f,
                                      100.0f);
        view = camera.GetViewMatrix();

        // field
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        fieldShader.use();
        glBindVertexArray(fieldVAO);

        model = glm::mat4(1.0f);
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::translate(model,
                               glm::vec3(0.0f, 0.0f, -5.0f));
        model = glm::scale(model,
                           glm::vec3(20.0f, 20.0f, 1.0f));

        fieldShader.setMat4("model", model);
        fieldShader.setMat4("view", view);
        fieldShader.setMat4("projection", projection);

        fieldShader.setVec3("dirLight.direction", dirLightDirection.x, dirLightDirection.y, dirLightDirection.z);

        fieldShader.setVec3("dirLight.ambient", dirLightAmbient.x, dirLightAmbient.y, dirLightAmbient.z);
        fieldShader.setVec3("dirLight.diffuse", dirLightDiffuse.x, dirLightDiffuse.y, dirLightDiffuse.z);
        fieldShader.setVec3("dirLight.specular", dirLightSpecular.x, dirLightSpecular.y, dirLightSpecular.z);

        fieldShader.setVec3("pointLight.position", pointLightPosition.x, pointLightPosition.y, pointLightPosition.z);
        fieldShader.setVec3("pointLight.ambient", pointLightAmbient.x, pointLightAmbient.y, pointLightAmbient.z);
        fieldShader.setVec3("pointLight.diffuse", pointLightDiffuse.x, pointLightDiffuse.y, pointLightDiffuse.z);
        fieldShader.setVec3("pointLight.specular", pointLightSpecular.x, pointLightSpecular.y, pointLightSpecular.z);
        fieldShader.setFloat("pointLight.constant", 1.0f);
        fieldShader.setFloat("pointLight.linear", 0.027f);
        fieldShader.setFloat("pointLight.quadratic", 0.0028f);

        fieldShader.setVec3("viewPos", camera.Position);

        fieldShader.setFloat("material.shininess", 0.0f);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, fieldDiffuseMap);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // clouds
        cloudShader.use();
        glBindVertexArray(cloudVAO);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, cloudTexture);

        cloudShader.setMat4("view", view);
        cloudShader.setMat4("projection", projection);
        for (unsigned int i = 0; i < clouds.size(); i++) {
            model = glm::mat4(1.0f);
            model = glm::translate(model, clouds[i]);
            model = glm::scale(model, glm::vec3(5.0f, 2.0f, 0.0f));
            cloudShader.setMat4("model", model);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        }

        glEnable(GL_CULL_FACE); // face culling
        glCullFace(GL_BACK); // default, but let's emphasize it

        // lamp
        lampShader.use();

        model = glm::mat4(1.0f);
        model = glm::translate(model,
                               glm::vec3(10.0f, -5.0f, 0.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(0.2f));
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        lampShader.setMat4("model", model);
        lampShader.setMat4("view", view);
        lampShader.setMat4("projection", projection);

        lampShader.setVec3("dirLight.direction", dirLightDirection.x, dirLightDirection.y, dirLightDirection.z);

        lampShader.setVec3("dirLight.ambient", dirLightAmbient.x, dirLightAmbient.y, dirLightAmbient.z);
        lampShader.setVec3("dirLight.diffuse", dirLightDiffuse.x, dirLightDiffuse.y, dirLightDiffuse.z);
        lampShader.setVec3("dirLight.specular", dirLightSpecular.x, dirLightSpecular.y, dirLightSpecular.z);

        lampShader.setVec3("viewPos", camera.Position);

        lampShader.setFloat("material.shininess", 256.0f);

        lampModel.Draw(lampShader);

        // football
        footballShader.use();

        model = glm::mat4(1.0f);
        model = glm::translate(model,
                               glm::vec3(0.0f, -3.9f, glm::sin(glfwGetTime()) *
                                                      15.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(10.0f));
        model = glm::rotate(model, (float) glm::sin(glfwGetTime()) * 15.0f, glm::vec3(1.0f, 0.0f, 0.0f));
        footballShader.setMat4("model", model);
        footballShader.setMat4("view", view);
        footballShader.setMat4("projection", projection);

        footballShader.setVec3("dirLight.direction", dirLightDirection.x, dirLightDirection.y, dirLightDirection.z);

        footballShader.setVec3("dirLight.ambient", dirLightAmbient.x, dirLightAmbient.y, dirLightAmbient.z);
        footballShader.setVec3("dirLight.diffuse", dirLightDiffuse.x, dirLightDiffuse.y, dirLightDiffuse.z);
        footballShader.setVec3("dirLight.specular", dirLightSpecular.x, dirLightSpecular.y, dirLightSpecular.z);

        footballShader.setVec3("pointLight.position", pointLightPosition.x, pointLightPosition.y, pointLightPosition.z);
        footballShader.setVec3("pointLight.ambient", pointLightAmbient.x, pointLightAmbient.y, pointLightAmbient.z);
        footballShader.setVec3("pointLight.diffuse", pointLightDiffuse.x, pointLightDiffuse.y, pointLightDiffuse.z);
        footballShader.setVec3("pointLight.specular", pointLightSpecular.x, pointLightSpecular.y, pointLightSpecular.z);
        footballShader.setFloat("pointLight.constant", 1.0f);
        footballShader.setFloat("pointLight.linear", 0.027f);
        footballShader.setFloat("pointLight.quadratic", 0.0028f);

        footballShader.setVec3("viewPos", camera.Position);

        footballShader.setFloat("material.shininess", 256.0f);

        footballModel.Draw(footballShader);

        glDisable(GL_CULL_FACE);

        // skybox
        glDepthFunc(GL_LEQUAL);

        skyboxShader.use();

        view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
        skyboxShader.setMat4("view", view);
        skyboxShader.setMat4("projection", projection);

        glBindVertexArray(skyboxVAO);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glDepthMask(GL_TRUE);

        // swapping buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(__attribute__((unused)) GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

void mouse_callback(__attribute__((unused)) GLFWwindow *window, double xPos, double yPos) {
    if (firstMouse) {
        lastX = xPos;
        lastY = yPos;
        firstMouse = false;
    }

    float xOffset = xPos - lastX;
    float yOffset = lastY - yPos;

    lastX = xPos;
    lastY = yPos;

    camera.ProcessMouseMovement(xOffset, yOffset);
}

void
scroll_callback(__attribute__((unused)) GLFWwindow *window, __attribute__((unused)) double xOffset, double yOffset) {
    camera.ProcessMouseScroll(yOffset);
}

unsigned int loadCubemap(vector<std::string> faces) {
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++) {
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                         0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
            );
            stbi_image_free(data);
        } else {
            std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}

unsigned int loadTexture(char const *path) {
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data) {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);


        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    } else {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}