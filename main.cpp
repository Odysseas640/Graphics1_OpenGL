#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/filesystem.h>
#include <learnopengl/shader_m.h>
#include <learnopengl/camera.h>
#include <learnopengl/model.h>

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window, float&, float&, float&, float&, float&, float&, float&, float&, float&, float&, float&, float&, float&, float&, unsigned int&, float&, float&);
unsigned int loadCubemap(vector<std::string> faces);
unsigned int loadTexture(const char* path);

// settings
const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 900;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;
//glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

int main()
{
    // glfw: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "GraficsAssignment", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    //stbi_set_flip_vertically_on_load(true);

    // configure global opengl state
    glEnable(GL_DEPTH_TEST);

    // build and compile shaders
    Shader planetShader("planetShader.vs", "planetShader.fs");
    Shader lightingShader("cubesLightingShader.vs", "cubesLightingShader.fs");
    Shader skyboxShader("skyboxShader.vs", "skyboxShader.fs");

    // load models
    Model planetModel(FileSystem::getPath("resources/planet/planet.obj"));
    
    ////////  CUBES STUFF
    float vertices[] = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
    };
    // first, configure the cube's VAO (and VBO)
    unsigned int VBO, cubeVAO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(cubeVAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    // second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
    unsigned int lightCubeVAO;
    glGenVertexArrays(1, &lightCubeVAO);
    glBindVertexArray(lightCubeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // note that we update the lamp's position attribute's stride to reflect the updated buffer data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    unsigned int diffuseMap = loadTexture(FileSystem::getPath("resources/container.png").c_str());
    lightingShader.use();
    lightingShader.setInt("material.diffuse", 0);
    /////////  CUBES STUFF END
    //////////////////////////////////////////////////////////////        SKYBOX STUFF
    float skyboxVertices[] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };
    // skybox VAO
    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    vector<std::string> faces
    {
        FileSystem::getPath("resources/costelacion1.jpg"), // Right
        FileSystem::getPath("resources/costelacion1.jpg"), // Left
        FileSystem::getPath("resources/costelacion1.jpg"),
        FileSystem::getPath("resources/costelacion1.jpg"),
        FileSystem::getPath("resources/costelacion1.jpg"), // Front
        FileSystem::getPath("resources/costelacion1.jpg") // Back
    };
    unsigned int cubemapTexture = loadCubemap(faces);
    skyboxShader.use();
    skyboxShader.setInt("skybox", 0);
    ////////////////////////////////////////////////////////       SKYBOX STUFF END

    // render loop
    ////////////////////////
    float planetAngle = 0.0f;
    float cube1Angle = 0.0f;
    float cube2Angle = 0.0f;
    float cube3Angle = 0.0f;
    float cube4Angle = 0.0f;
    float cube5Angle = 0.0f;
    float cube6Angle = 0.0f;
    float planetSpeed = 0.000002f;
    float cube1Speed = 0.000001f;
    float cube2Speed = 0.000001f;
    float cube3Speed = -0.000002f;
    float cube4Speed = 0.000011f;
    float cube5Speed = -0.000009f;
    float cube6Speed = 0.000007f;
    float planetSpin = 0.0f;
    float cube1Spin = 0.0f;
    float cube2Spin = 0.0f;
    float cube3Spin = 0.0f;
    float cube4Spin = 0.0f;
    float cube5Spin = 0.0f;
    float cube6Spin = 0.0f;
    float planetSpinSpeed = 0.0000f;
    float cube1SpinSpeed = 0.000015f;
    float cube2SpinSpeed = 0.000018f;
    float cube3SpinSpeed = 0.00002f;
    float cube4SpinSpeed = 0.0001f;
    float cube5SpinSpeed = 0.00017f;
    float cube6SpinSpeed = 0.00008f;
    float X = 0.0f;
    float Z = 0.0f;
    float viewX = 0.0f;
    float viewY = 0.0f;
    /////////////////////////
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        processInput(window, planetSpeed, cube1Speed, cube2Speed, cube3Speed, cube4Speed, cube5Speed, cube6Speed, planetSpinSpeed, cube1SpinSpeed, cube2SpinSpeed, cube3SpinSpeed, cube4SpinSpeed, cube5SpinSpeed, cube6SpinSpeed, diffuseMap, viewX, viewY);

        // render
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // don't forget to enable shader before setting uniforms
        planetShader.use();

        // view/projection transformations
        planetShader.setVec3("objectColor", 1.0f, 1.0f, 1.0f);
        planetShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
        planetShader.setVec3("lightPos", 0.0f, 0.0f, 0.0f);
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        view = glm::rotate(view, viewX, glm::vec3(1, 0, 0));
        view = glm::rotate(view, viewY, glm::vec3(0, 1, 0));
        planetShader.setMat4("projection", projection);
        planetShader.setMat4("view", view);

        glm::mat4 model = glm::mat4(1.0f);

        /////////////////////////////////
        planetAngle += planetSpeed;
        planetSpin += planetSpinSpeed;
        X = 2 * sin(planetAngle);
        Z = 2 * cos(planetAngle);
        model = glm::translate(model, glm::vec3(X, 0, Z)); // Move to the correct position in the planet's orbit
        model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));	// Scale planet down
        model = glm::rotate(model, planetSpin, glm::vec3(0, 0, 1));
        model = glm::rotate(model, 1.57f, glm::vec3(1, 0, 0));
        planetShader.setMat4("model", model);
        planetModel.Draw(planetShader);
        //undo rotation
        model = glm::rotate(model, -1.57f, glm::vec3(1, 0, 0));
        model = glm::rotate(model, -planetSpin, glm::vec3(0, 0, 1));
        model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));

        glm::vec3 lightPos(X, 0, Z); // Light source is at the center of the planet

        // Draw 1st cube
        cube1Angle += cube1Speed;
        cube1Spin += cube1SpinSpeed;
        X = 5 * sin(cube1Angle);
        Z = 5 * cos(cube1Angle);
        model = glm::translate(model, glm::vec3(X, 0, Z)); // Move to this cube's position
        model = glm::rotate(model, cube1Spin, glm::vec3(0, 0, 1));
        //////////////////////////////////////// Draw CUBE
        lightingShader.use();
        lightingShader.setVec3("light.position", lightPos);
        lightingShader.setVec3("viewPos", camera.Position); // light properties
        lightingShader.setVec3("light.ambient", 0.3f, 0.3f, 0.3f);
        lightingShader.setVec3("light.diffuse", 0.5f, 0.5f, 0.5f);
        lightingShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
        lightingShader.setVec3("material.specular", 0.8f, 0.8f, 0.8f); // material properties
        lightingShader.setFloat("material.shininess", 64.0f);
        lightingShader.setMat4("projection", projection);
        lightingShader.setMat4("view", view);
        lightingShader.setMat4("model", model);
        glActiveTexture(GL_TEXTURE0); // bind diffuse map
        glBindTexture(GL_TEXTURE_2D, diffuseMap);
        glBindVertexArray(cubeVAO); // render the cube
        glDrawArrays(GL_TRIANGLES, 0, 36);
        //////////////////////////////////////// END DRAW CUBE
        // Undo transformations to draw next cube independently
        model = glm::rotate(model, -cube1Spin, glm::vec3(0, 0, 1));
        model = glm::translate(model, glm::vec3(-X, 0, -Z));

        // Draw 2nd cube
        cube2Angle += cube2Speed;
        cube2Spin += cube2SpinSpeed;
        X = 7 * sin(cube2Angle);
        Z = 7 * cos(cube2Angle);
        model = glm::translate(model, glm::vec3(X, X, Z));
        model = glm::rotate(model, cube2Spin, glm::vec3(1, 0, 0));
        //////////////////////////////////////// Draw CUBE
        lightingShader.setMat4("projection", projection);
        lightingShader.setMat4("view", view);
        lightingShader.setMat4("model", model);
        glActiveTexture(GL_TEXTURE0); // bind diffuse map
        glBindTexture(GL_TEXTURE_2D, diffuseMap);
        glBindVertexArray(cubeVAO); // render the cube
        glDrawArrays(GL_TRIANGLES, 0, 36);
        //////////////////////////////////////// END DRAW CUBE
        model = glm::rotate(model, -cube2Spin, glm::vec3(1, 0, 0));
        model = glm::translate(model, glm::vec3(-X, -X, -Z));

        // Draw 3rd cube
        cube3Angle += cube3Speed;
        cube3Spin += cube3SpinSpeed;
        X = 9 * sin(cube3Angle);
        Z = 9 * cos(cube3Angle);
        model = glm::translate(model, glm::vec3(X, Z, 0)); // Move to this cube's position
        model = glm::rotate(model, cube3Spin, glm::vec3(1, 1, 1));
        //////////////////////////////////////// Draw CUBE
        lightingShader.setMat4("projection", projection);
        lightingShader.setMat4("view", view);
        lightingShader.setMat4("model", model);
        glActiveTexture(GL_TEXTURE0); // bind diffuse map
        glBindTexture(GL_TEXTURE_2D, diffuseMap);
        glBindVertexArray(cubeVAO); // render the cube
        glDrawArrays(GL_TRIANGLES, 0, 36);
        //////////////////////////////////////// END DRAW CUBE
        model = glm::rotate(model, -cube3Spin, glm::vec3(1, 1, 1));
        model = glm::translate(model, glm::vec3(-X, -Z, 0));

        // Draw 4th cube
        cube4Angle += cube4Speed;
        cube4Spin += cube4SpinSpeed;
        X = 10 * sin(cube4Angle);
        Z = 12.5 * cos(cube4Angle);
        model = glm::translate(model, glm::vec3(X, Z, Z)); // Move to this cube's position
        model = glm::rotate(model, cube4Spin, glm::vec3(0, 1, 0));
        //////////////////////////////////////// Draw CUBE
        lightingShader.setMat4("projection", projection);
        lightingShader.setMat4("view", view);
        lightingShader.setMat4("model", model);
        glActiveTexture(GL_TEXTURE0); // bind diffuse map
        glBindTexture(GL_TEXTURE_2D, diffuseMap);
        glBindVertexArray(cubeVAO); // render the cube
        glDrawArrays(GL_TRIANGLES, 0, 36);
        //////////////////////////////////////// END DRAW CUBE
        model = glm::rotate(model, -cube4Spin, glm::vec3(0, 1, 0));
        model = glm::translate(model, glm::vec3(-X, -Z, -Z));

        // Draw 5th cube
        cube5Angle += cube5Speed;
        cube5Spin += cube5SpinSpeed;
        X = 13.5 * sin(cube5Angle);
        Z = 16 * cos(cube5Angle);
        model = glm::translate(model, glm::vec3(X, -X, Z)); // Move to this cube's position
        model = glm::rotate(model, cube5Spin, glm::vec3(6, 9, 1));
        //////////////////////////////////////// Draw CUBE
        lightingShader.setMat4("projection", projection);
        lightingShader.setMat4("view", view);
        lightingShader.setMat4("model", model);
        glActiveTexture(GL_TEXTURE0); // bind diffuse map
        glBindTexture(GL_TEXTURE_2D, diffuseMap);
        glBindVertexArray(cubeVAO); // render the cube
        glDrawArrays(GL_TRIANGLES, 0, 36);
        //////////////////////////////////////// END DRAW CUBE
        model = glm::rotate(model, -cube5Spin, glm::vec3(6, 9, 1));
        model = glm::translate(model, glm::vec3(-X, X, -Z));

        // Draw 6th cube
        cube6Angle += cube6Speed;
        cube6Spin += cube6SpinSpeed;
        X = 16 * sin(cube6Angle);
        Z = 16 * cos(cube6Angle);
        model = glm::translate(model, glm::vec3(X, 0, Z)); // Move to this cube's position
        model = glm::rotate(model, cube6Spin, glm::vec3(1, 5, 1));
        //////////////////////////////////////// Draw CUBE
        lightingShader.setMat4("projection", projection);
        lightingShader.setMat4("view", view);
        lightingShader.setMat4("model", model);
        glActiveTexture(GL_TEXTURE0); // bind diffuse map
        glBindTexture(GL_TEXTURE_2D, diffuseMap);
        glBindVertexArray(cubeVAO); // render the cube
        glDrawArrays(GL_TRIANGLES, 0, 36);
        //////////////////////////////////////// END DRAW CUBE
        model = glm::rotate(model, -cube6Spin, glm::vec3(1, 5, 1));
        model = glm::translate(model, glm::vec3(-X, 0, -Z));

        // draw skybox as last
        glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
        skyboxShader.use();
        view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
        skyboxShader.setMat4("view", view);
        skyboxShader.setMat4("projection", projection);
        // skybox cube
        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS); // set depth function back to default

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window, float &planetSpeed, float &cube1Speed, float &cube2Speed, float& cube3Speed, float& cube4Speed, float& cube5Speed, float& cube6Speed, float& planetSpinSpeed, float& cube1SpinSpeed, float& cube2SpinSpeed, float& cube3SpinSpeed, float& cube4SpinSpeed, float& cube5SpinSpeed, float& cube6SpinSpeed, unsigned int& diffuseMap, float& viewX, float& viewY) {
    static int space_pressed = 0, backspace_pressed = 0;
    static int texture_loaded = 0, other_texture;
    if (glfwGetKey(window, GLFW_KEY_SPACE) != GLFW_PRESS)
        space_pressed = 0;
    if (glfwGetKey(window, GLFW_KEY_BACKSPACE) != GLFW_PRESS) {
        //cout << "backspace not pressed" << endl;
        backspace_pressed = 0;
    }

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        while (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) != GLFW_PRESS && glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS) {
            glfwWaitEvents();
        }
    }

    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
        planetSpeed = 0.000001f;
        cube1Speed = 0.000001f;
        cube2Speed = 0.000001f;
        cube3Speed = 0.000001f;
        cube4Speed = 0.000001f;
        cube5Speed = 0.000001f;
        cube6Speed = 0.000001f;
        planetSpinSpeed = 0.00001f;
        cube1SpinSpeed = 0.00001f;
        cube2SpinSpeed = 0.00001f;
        cube3SpinSpeed = 0.00001f;
        cube4SpinSpeed = 0.00001f;
        cube5SpinSpeed = 0.00001f;
        cube6SpinSpeed = 0.00001f;
        viewX = 0;
        viewY = 0;
    }

    if (glfwGetKey(window, GLFW_KEY_BACKSPACE) == GLFW_PRESS && backspace_pressed == 0) {
        cout << "Change texture" << endl;
        if (texture_loaded == 0) {
            other_texture = diffuseMap;
            diffuseMap = loadTexture(FileSystem::getPath("resources/Doge.jpg").c_str());
            texture_loaded = 1;
        }
        else {
            unsigned int temp = diffuseMap;
            diffuseMap = other_texture;
            other_texture = temp;
        }
        backspace_pressed = 1;
    }

    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) != GLFW_PRESS) {
        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
            cube1Speed += 0.0000001;
        else if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
            cube1Speed -= 0.0000001;
        if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
            cube2Speed += 0.0000001;
        else if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
            cube2Speed -= 0.0000001;
        if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS)
            cube3Speed += 0.0000001;
        else if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS)
            cube3Speed -= 0.0000001;
        if (glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS)
            cube4Speed += 0.0000001;
        else if (glfwGetKey(window, GLFW_KEY_8) == GLFW_PRESS)
            cube4Speed -= 0.0000001;
        if (glfwGetKey(window, GLFW_KEY_9) == GLFW_PRESS)
            cube5Speed += 0.0000001;
        else if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS)
            cube5Speed -= 0.0000001;
        if (glfwGetKey(window, GLFW_KEY_MINUS) == GLFW_PRESS)
            cube6Speed += 0.0000001;
        else if (glfwGetKey(window, GLFW_KEY_EQUAL) == GLFW_PRESS)
            cube6Speed -= 0.0000001;
        if (glfwGetKey(window, GLFW_KEY_LEFT_BRACKET) == GLFW_PRESS)
            planetSpeed += 0.00000005;
        else if (glfwGetKey(window, GLFW_KEY_RIGHT_BRACKET) == GLFW_PRESS)
            planetSpeed -= 0.00000005;
    }
    else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
            cube1SpinSpeed += 0.0000001;
        else if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
            cube1SpinSpeed -= 0.0000001;
        if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
            cube2SpinSpeed += 0.0000001;
        else if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
            cube2SpinSpeed -= 0.0000001;
        if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS)
            cube3SpinSpeed += 0.0000001;
        else if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS)
            cube3SpinSpeed -= 0.0000001;
        if (glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS)
            cube4SpinSpeed += 0.0000001;
        else if (glfwGetKey(window, GLFW_KEY_8) == GLFW_PRESS)
            cube4SpinSpeed -= 0.0000001;
        if (glfwGetKey(window, GLFW_KEY_9) == GLFW_PRESS)
            cube5SpinSpeed += 0.0000001;
        else if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS)
            cube5SpinSpeed -= 0.0000001;
        if (glfwGetKey(window, GLFW_KEY_MINUS) == GLFW_PRESS)
            cube6SpinSpeed += 0.0000001;
        else if (glfwGetKey(window, GLFW_KEY_EQUAL) == GLFW_PRESS)
            cube6SpinSpeed -= 0.0000001;
        if (glfwGetKey(window, GLFW_KEY_LEFT_BRACKET) == GLFW_PRESS)
            planetSpinSpeed += 0.0000001;
        else if (glfwGetKey(window, GLFW_KEY_RIGHT_BRACKET) == GLFW_PRESS)
            planetSpinSpeed -= 0.0000001;
    }

    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) { // Move + LShift = sprint
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            camera.ProcessKeyboard(FORWARD, 2*deltaTime);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            camera.ProcessKeyboard(BACKWARD, 2*deltaTime);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            camera.ProcessKeyboard(LEFT, 2*deltaTime);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            camera.ProcessKeyboard(RIGHT, 2*deltaTime);
    }
    else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) != GLFW_PRESS) {
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            camera.ProcessKeyboard(FORWARD, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            camera.ProcessKeyboard(BACKWARD, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            camera.ProcessKeyboard(LEFT, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            camera.ProcessKeyboard(RIGHT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        viewX += deltaTime;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        viewX -= deltaTime;
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        viewY += deltaTime;
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        viewY -= deltaTime;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    //camera.ProcessMouseScroll(yoffset);
}

unsigned int loadCubemap(vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
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
// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture(char const* path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
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
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}