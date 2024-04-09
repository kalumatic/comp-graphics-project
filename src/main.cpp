// TODO
// blending, shadows

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <learnopengl/filesystem.h>
#include <learnopengl/shader_m.h>
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
const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 800;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;    // time between current frame and last frame
float lastFrame = 0.0f;


int main() {
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

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

//    stbi_set_flip_vertically_on_load(true);

    glEnable(GL_DEPTH_TEST);

//    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // FLAT TERRAIN

    float vertices[] = {
            // positions                // normals                      // texture coords
            0.5f, 0.8f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, // top right
            0.5f, -0.8f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom right
            -0.5f, -0.8f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, // bottom left
            -0.5f, 0.8f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f  // top left

    };

    unsigned int indices[] = {
            0, 1, 3, // first triangle
            1, 2, 3  // second triangle
    };

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

    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // skybox
    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) 0);

//    unsigned int terrainTexture;
//    glGenTextures(1, &terrainTexture);
//    glBindTexture(GL_TEXTURE_2D,
//                  terrainTexture); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
//    // set the texture wrapping parameters
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
//                    GL_REPEAT);    // set texture wrapping to GL_REPEAT (default wrapping method)
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//    // set texture filtering parameters
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//    // load image, create texture and generate mipmaps
//    int width, height, nrChannels;
//    // The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
//    unsigned char *data = stbi_load(FileSystem::getPath("resources/textures/field.jpeg").c_str(), &width, &height,
//                                    &nrChannels, 0);
//    if (data) {
//        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
//        glGenerateMipmap(GL_TEXTURE_2D);
//    } else {
//        std::cout << "Failed to load texture" << std::endl;
//    }

    // skybox
    vector<std::string> faces
            {
                    "resources/textures/field-skyboxes/Footballfield2/posx.jpg",
                    "resources/textures/field-skyboxes/Footballfield2/negx.jpg",
                    "resources/textures/field-skyboxes/Footballfield2/posy.jpg",
                    "resources/textures/field-skyboxes/Footballfield2/negy.jpg",
                    "resources/textures/field-skyboxes/Footballfield2/posz.jpg",
                    "resources/textures/field-skyboxes/Footballfield2/negz.jpg"
//                    "resources/textures/skybox/right.jpg",
//                    "resources/textures/skybox/left.jpg",
//                    "resources/textures/skybox/top.jpg",
//                    "resources/textures/skybox/bottom.jpg",
//                    "resources/textures/skybox/front.jpg",
//                    "resources/textures/skybox/back.jpg"
            };
    unsigned int cubemapTexture = loadCubemap(faces);

//    stbi_image_free(data);


    // build and compile shaders
    // -------------------------
    Shader terrainShader("resources/shaders/terrain.vs", "resources/shaders/terrain.fs");

    unsigned int diffuseMap = loadTexture(FileSystem::getPath("resources/textures/field.jpeg").c_str());
    unsigned int specularMap = loadTexture(FileSystem::getPath("resources/textures/black.jpg").c_str());
    terrainShader.setInt("material.diffuse", 0);
    terrainShader.setInt("material.specular", 1);

    terrainShader.setFloat("material.shininess", 1.0f);

//    Shader baseballShader("resources/shaders/backpack.vs", "resources/shaders/backpack.fs");
    Shader footballShader("resources/shaders/football.vs", "resources/shaders/football.fs");

    Shader skyboxShader("resources/shaders/skybox.vs", "resources/shaders/skybox.fs");
    skyboxShader.use();
    skyboxShader.setInt("skybox", 0);

    // load models
    // -----------
//    Model baseballModel(FileSystem::getPath("resources/objects/backpack/backpack.obj"));
    Model footballModel(FileSystem::getPath("resources/objects/uploads_files_824560_Cafusa_obj/Cafusa_Base_Mesh.obj"));

    footballModel.SetShaderTextureNamePrefix("material.");

    while (!glfwWindowShouldClose(window)) {
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(0.0f, 0.2f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        // terrain
        terrainShader.use();
//        glBindTexture(GL_TEXTURE_2D, terrainTexture);
        glBindVertexArray(VAO);

        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float) SCR_WIDTH / (float) SCR_HEIGHT, 0.1f,
                                                100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        terrainShader.setMat4("projection", projection);
        terrainShader.setMat4("view", view);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::translate(model,
                               glm::vec3(0.0f, 0.0f, -5.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model,
                           glm::vec3(20.0f, 20.0f, 1.0f));
        terrainShader.setMat4("model", model);

        terrainShader.setVec3("dirLight.direction", -1.0f, -1.0f, 0.0f);
        terrainShader.setVec3("viewPos", camera.Position);

        // light properties
        terrainShader.setVec3("dirLight.ambient", 0.2f, 0.2f, 0.2f);
        terrainShader.setVec3("dirLight.diffuse", 0.6f, 0.6f, 0.6f);
        terrainShader.setVec3("dirLight.specular", 0.0f, 0.0f, 0.0f);

        // material properties
        terrainShader.setFloat("material.shininess", 64.0f);

        // bind diffuse map
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseMap);
        // bind specular map
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specularMap);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // baseball
//        baseballShader.use();
//
//        baseballShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
//        baseballShader.setVec3("viewPos", camera.Position);
//
//        // light properties
//        baseballShader.setVec3("dirLight.ambient", 0.2f, 0.2f, 0.2f);
//        baseballShader.setVec3("dirLight.diffuse", 0.5f, 0.5f, 0.5f);
//        baseballShader.setVec3("dirLight.specular", 1.0f, 1.0f, 1.0f);
//
//        // material properties
//        baseballShader.setFloat("material.shininess", 32.0f);
//
//
//        // view/projection transformations
//        projection = glm::perspective(glm::radians(camera.Zoom), (float) SCR_WIDTH / (float) SCR_HEIGHT, 0.1f,
//                                      100.0f);
//        view = camera.GetViewMatrix();
//        baseballShader.setMat4("projection", projection);
//        baseballShader.setMat4("view", view);
//
//        // render the loaded model
//        model = glm::mat4(1.0f);
//        model = glm::translate(model,
//                               glm::vec3(0.0f, 0.0f, -5.0f)); // translate it down so it's at the center of the scene
//        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));    // it's a bit too big for our scene, so scale it down
//        baseballShader.setMat4("model", model);
//        baseballModel.Draw(baseballShader);

        // football
//        glBindTexture(GL_TEXTURE_2D, 0);
        // face culling
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK); // default
        footballShader.use();

        // view/projection transformations
        projection = glm::perspective(glm::radians(camera.Zoom), (float) SCR_WIDTH / (float) SCR_HEIGHT, 0.1f,
                                      100.0f);
        view = camera.GetViewMatrix();
        footballShader.setMat4("projection", projection);
        footballShader.setMat4("view", view);

        model = glm::mat4(1.0f);
        model = glm::translate(model,
                               glm::vec3(0.0f, -3.9f, glm::sin(glfwGetTime()) *
                                                      15.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(10.0f));
        model = glm::rotate(model, (float) glm::sin(glfwGetTime()) * 15.0f, glm::vec3(1.0f, 0.0f, 0.0f));
        footballShader.setMat4("model", model);

        footballShader.setVec3("dirLight.direction", -1.0f, -1.0f, 0.0f);
        footballShader.setVec3("viewPos", camera.Position);

        // light properties
        footballShader.setVec3("dirLight.ambient", 0.2f, 0.2f, 0.2f);
        footballShader.setVec3("dirLight.diffuse", 0.6f, 0.6f, 0.6f);
        footballShader.setVec3("dirLight.specular", 1.0f, 1.0f, 1.0f);

        // material properties
        footballShader.setFloat("material.shininess", 128.0f);

        footballModel.Draw(footballShader);
        glDisable(GL_CULL_FACE);

        // skybox
        glDepthFunc(GL_LEQUAL);
        skyboxShader.use();

        projection = glm::perspective(glm::radians(camera.Zoom), (float) SCR_WIDTH / (float) SCR_HEIGHT, 0.1f,
                                      100.0f);
        view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
        skyboxShader.setMat4("projection", projection);
        skyboxShader.setMat4("view", view);

        glBindVertexArray(skyboxVAO);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glDepthMask(GL_TRUE);


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

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(__attribute__((unused)) GLFWwindow *window, double xPos, double yPos) {
    if (firstMouse) {
        lastX = xPos;
        lastY = yPos;
        firstMouse = false;
    }

    float xOffset = xPos - lastX;
    float yOffset = lastY - yPos; // reversed since y-coordinates go from bottom to top

    lastX = xPos;
    lastY = yPos;

    camera.ProcessMouseMovement(xOffset, yOffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
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