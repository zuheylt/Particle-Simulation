#include <math.h>
#include <SDL2/SDL.h>
#include <stdlib.h>
#include <time.h>
typedef struct {
    float x, y;
    float vx, vy;
    float ax, ay;
}Particle;
#define RADIUS  2
#define NUM_PARTICLES 500
#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080
Particle particles[NUM_PARTICLES];

float distance(Particle p1, Particle p2){
    return sqrtf((p1.x-p2.x)*(p1.x-p2.x)+(p1.y-p2.y)*(p1.y-p2.y));
}


void initiate_particles(Particle particles[], int num_particles){
    for (int i= 0; i<num_particles; i++){
        int valid;
        do{
            valid=1;
            particles[i].x = rand()%(SCREEN_WIDTH-RADIUS);
            particles[i].y = rand()%(SCREEN_HEIGHT-RADIUS);
            
            for (int j = 0; j<i; j++){
                if (distance(particles[i],particles[j])< (2*RADIUS+1)){
                    valid =0;
                }
            }
        }while(!valid);
        particles[i].vx=0;
        particles[i].vy=0;
        particles[i].ax=0;
        particles[i].ay=0;

    }
}
void draw_circle(SDL_Renderer *renderer, Particle particles[]){
    for (int i=0; i<NUM_PARTICLES; i++){
        for (int w=0; w< RADIUS*2;w++){
            for (int h=0; h<RADIUS*2;h++){
                int dx = RADIUS-w;
                int dy = RADIUS-h;
                if (dx*dx+dy*dy<= RADIUS*RADIUS){
                SDL_RenderDrawPoint(renderer, (int) particles[i].x+dx, (int) particles[i].y+dy);
                }
            }
        }
    } 
}


int main(){
    srand(time(NULL));
    if (SDL_Init(SDL_INIT_VIDEO)!= 0){
        printf("SDL couldn't initialized SDL_ERROR: %s",SDL_GetError());
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("Static Particle Simulation", 
                                            SDL_WINDOWPOS_CENTERED,
                                            SDL_WINDOWPOS_CENTERED,
                                            SCREEN_WIDTH,
                                            SCREEN_HEIGHT,
                                            SDL_WINDOW_FULLSCREEN);
    if (!window){
        printf("window couldn't created SDL_ERROR: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if (!renderer){
        printf("Couldn't created window SDL_ERROR: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    } 

    Particle particles[NUM_PARTICLES];
    initiate_particles(particles,NUM_PARTICLES);



    int running =1;
    SDL_Event event;

    while(running){
        while (SDL_PollEvent(&event)){
            if (event.type == SDL_QUIT){
                running =0;
            }
        }

        SDL_SetRenderDrawColor(renderer,0,0,0,255);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer,255,255,255,255);
        draw_circle(renderer,particles);
        SDL_RenderPresent(renderer);    




    }



    return 0;
}
