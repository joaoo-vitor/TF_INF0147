#version 330 core

// Atributos de vértice recebidos como entrada ("in") pelo Vertex Shader.
// Veja a função BuildTrianglesAndAddToVirtualScene() em "main.cpp".
layout (location = 0) in vec4 model_coefficients;
layout (location = 1) in vec4 normal_model;
layout (location = 2) in vec2 texture_coefficients;

// Matrizes computadas no código C++ e enviadas para a GPU
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform int object_id;
uniform sampler2D TextureImage5;
uniform sampler2D TextureImage6;
uniform vec4 bbox_min;
uniform vec4 bbox_max;

// Atributos de vértice que serão gerados como saída ("out") pelo Vertex Shader.
// ** Estes serão interpolados pelo rasterizador! ** gerando, assim, valores
// para cada fragmento, os quais serão recebidos como entrada pelo Fragment
// Shader. Veja o arquivo "shader_fragment.glsl".
out vec4 position_world;
out vec4 position_model;
out vec4 normal;
out vec2 texcoords;
out vec4 normal_modelspace;
out vec3 color_v;

// Identificador que define qual objeto está sendo desenhado no momento
#define PLANE 0
#define SKYBOX 1
#define CAR_BODY 2
#define CAR_PLAQUES 3
#define CAR_TYRES 4
#define CAR_TYRES_BACK 5
#define CAR_GLASSES 6
#define TRACK 7
#define GROUND 8
#define EDGE 9
#define LAMP 10
#define FINISH_LINE 11

#define M_PI 3.14159265359

void main()
{
    // A variável gl_Position define a posição final de cada vértice
    // OBRIGATORIAMENTE em "normalized device coordinates" (NDC), onde cada
    // coeficiente estará entre -1 e 1 após divisão por w.
    // Veja {+NDC2+}.
    //
    // O código em "main.cpp" define os vértices dos modelos em coordenadas
    // locais de cada modelo (array model_coefficients). Abaixo, utilizamos
    // operações de modelagem, definição da câmera, e projeção, para computar
    // as coordenadas finais em NDC (variável gl_Position). Após a execução
    // deste Vertex Shader, a placa de vídeo (GPU) fará a divisão por W. Veja
    // slides 41-67 e 69-86 do documento Aula_09_Projecoes.pdf.

    gl_Position = projection * view * model * model_coefficients;

    // Como as variáveis acima  (tipo vec4) são vetores com 4 coeficientes,
    // também é possível acessar e modificar cada coeficiente de maneira
    // independente. Esses são indexados pelos nomes x, y, z, e w (nessa
    // ordem, isto é, 'x' é o primeiro coeficiente, 'y' é o segundo, ...):
    //
    //     gl_Position.x = model_coefficients.x;
    //     gl_Position.y = model_coefficients.y;
    //     gl_Position.z = model_coefficients.z;
    //     gl_Position.w = model_coefficients.w;
    //

    // Agora definimos outros atributos dos vértices que serão interpolados pelo
    // rasterizador para gerar atributos únicos para cada fragmento gerado.

    // Posição do vértice atual no sistema de coordenadas global (World).
    position_world = model * model_coefficients;

    // Posição do vértice atual no sistema de coordenadas local do modelo.
    position_model = model_coefficients;

    // Normal do vértice atual no sistema de coordenadas global (World).
    // Veja slides 123-151 do documento Aula_07_Transformacoes_Geometricas_3D.pdf.
    normal = inverse(transpose(model)) * normal_model;
    normal.w = 0.0;

    if(object_id == LAMP){
        float U;
        float V;

        vec3 I = vec3(1.0,1.0,1.0); // O espectro da fonte de luz
        vec3 Ia = vec3(0.2,0.2,0.2); // O espectro da luz ambiente
        vec3 Kd = vec3(0.0,0.0,0.0);; // Refletância difusa 
        vec3 Ka = vec3(0.1,0.1,0.1); // Refletância ambiente 

        vec4 l = normalize(vec4(1.0,1.0,0.0,0.0));
        vec4 n = normalize(normal);
        
        // Projeção cilindrica
        float angle = atan(position_model.x, position_model.z);
        U = (angle + M_PI)/ (2.0 * M_PI);
        V = position_model.y/1024*100;
        
        Kd = texture(TextureImage5, vec2(U,V)).rgb*(0.6+0.4*texture(TextureImage6, vec2(U,V)).rgb); 
        // vec3 lambert_diffuse_term = Kd * I * max(0, dot(n, l));
        vec3 lambert_diffuse_term = Kd * I;
        vec3 ambient_term = Ka * Ia; // O termo ambiente
        color_v = lambert_diffuse_term + ambient_term;
    }
    // Coordenadas de textura obtidas do arquivo OBJ (se existirem!)
    texcoords = texture_coefficients;
    normal_modelspace = normal_model;

}

