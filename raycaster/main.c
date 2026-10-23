#include "raylib.h"
#include "map.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define screenWidth 1200
#define screenHeight 800
#define textureWidth 64
#define textureHeight 64

#define ambientLight 5
#define numSprites 5

typedef struct Sprite{
    double posX, posY;
    Color *texture;
} Sprite;


Color* loadTexture(char* filename);


int main(void){

	// Initialiser map
	int map[DUNGEON_WIDTH][DUNGEON_HEIGHT]=
	{
	  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	  {1,0,0,0,0,0,2,2,2,2,2,0,0,0,0,3,0,3,0,3,0,0,0,1},
	  {1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,1},
	  {1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,3,0,0,0,3,0,0,0,1},
	  {1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,1},
	  {1,0,0,0,0,0,2,2,0,2,2,0,0,0,0,3,0,3,0,3,0,0,0,1},
	  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	  {1,4,4,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	  {1,4,0,4,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	  {1,4,0,0,0,0,5,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	  {1,4,0,4,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	  {1,4,0,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	  {1,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	  {1,4,4,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
	};

	// Charger les textures
	Color *wallTexture = loadTexture("wolftexs/redbrick.png");
	Color *floorTexture = loadTexture("wolftexs/greystone.png");
	Color *ceilingTexture = loadTexture("wolftexs/wood.png");
	Color *barrelTexture = loadTexture("wolftexs/barrel.png");
	Color *lampTexture = loadTexture("wolftexs/greenlight.png");
	Color *pillarTexture = loadTexture("wolftexs/pillar.png");
	Color *doorTexture = loadTexture("door.png");

	// Définir des sprites
	Sprite sprites[numSprites] = {
	    {2,2,barrelTexture},
		{15,10,pillarTexture},
		{5,3,barrelTexture},
		{18.3,12.1,lampTexture},
		{8.5,8.5,doorTexture},
	};

	double posX = 10;
	double posY = 10;

	double dirX = 0, dirY = 1;
	double planeX = 1, planeY = 0;



	// Raylib
	InitWindow(screenWidth, screenHeight, "Yugnad");
	SetTargetFPS(45);
	Color framebuffer[screenWidth * screenHeight];
	Image image = GenImageColor(screenWidth, screenHeight, BLACK);
	Texture2D texture = LoadTextureFromImage(image);
	UnloadImage(image);

	Image bloodImage = LoadImage("blood1.png");
    Texture2D blood1Tex = LoadTextureFromImage(bloodImage);
    bloodImage = LoadImage("blood2.png");
    Texture2D blood2Tex = LoadTextureFromImage(bloodImage);
    bloodImage = LoadImage("blood3.png");
    Texture2D blood3Tex = LoadTextureFromImage(bloodImage);



    UnloadImage(bloodImage);

    int bloodLevel = 0;


	while (!WindowShouldClose()){

		BeginDrawing();
		ClearBackground(BLACK);






		// FLOORCASTING
		for (int y=screenHeight/2 + 1 ; y<screenHeight ; y++){

    		//direction des rayons extremes gauche et droite
    		float rayDirX0 = dirX - planeX;
    		float rayDirY0 = dirY - planeY;
    		float rayDirX1 = dirX + planeX;
    		float rayDirY1 = dirY + planeY;

    		// position courante y par rapport à l'horizon
    		int p = y - screenHeight/2;
    		// position verticale camera
    		float posZ = 0.5 * screenHeight;
    		// distance de la ligne courante (dur à comprendre)
            // dessiner de profil :
            // posZ / rowDistance  =  p / norm(dir) = p / 1
    		float rowDistance = posZ / p;

            // calcul du pas pour chaque x
            float floorStepX = rowDistance * (rayDirX1 - rayDirX0) / screenWidth;
            float floorStepY = rowDistance * (rayDirY1 - rayDirY0) / screenWidth;
            // vrai coordonnées de la colonne gauche.
            float floorX = posX + rayDirX0 * rowDistance;
            float floorY = posY + rayDirY0 * rowDistance;

            for (int x=0 ; x<screenWidth ; x++){
                int cellX = (int)floorX;
                int cellY = (int)floorY;

                // obtenir les coordonnées du texel pertinent
                int tx = (int)(textureWidth * (floorX - cellX)) & (textureWidth - 1);
                int ty = (int)(textureHeight * (floorY - cellY)) & (textureHeight - 1);

                floorX += floorStepX;
                floorY += floorStepY;

                // y opposé verticalement pour dessin du plafond
                int oppositeY = (screenHeight/2)-(y-screenHeight/2);

                // obtenir les texels
                Color floorColor = floorTexture[tx + ty*textureWidth];
                Color ceilingColor = ceilingTexture[tx + ty*textureWidth];

                // assombrir en fonction de la distance
				double shade = 1.0 + rowDistance / (double)ambientLight;
				floorColor.r /= shade; floorColor.g /= shade; floorColor.b /= shade;
				ceilingColor.r /= shade; ceilingColor.g /= shade; ceilingColor.b /= shade;

                // dessin
                framebuffer[x + y*screenWidth] = floorColor;
                framebuffer[x +  oppositeY*screenWidth] = ceilingColor;

            }
		}







		// WALLCASTING
		double ZBuffer[screenWidth];
		for (int x=0; x<screenWidth ; x++){
			//calculer position et direction du rayon
			// cameraX : -1 à gauche, 0 au centre, 1 à droite
			double cameraX = 2*x / (double)screenWidth - 1;
			double rayDirX = dirX + planeX*cameraX;
			double rayDirY = dirY + planeY*cameraX;

			// variables DDA
			int mapX = (int)posX;
			int mapY = (int)posY;

			double sideDistX;
			double sideDistY;

			// croissance Y par unité de X
			double deltaDistX = (rayDirX==0) ?
			1e30 : fabs(1.0 / rayDirX);
			// croissance X par unité de Y
			double deltaDistY = (rayDirY==0) ?
			1e30 : fabs(1.0 / rayDirY);

			double perpWallDist;

			int stepX, stepY;

			int hit=0;
			int side; //NS ou EW ?

			// calculer step direction et sideDists initiales
			if (rayDirX<0){ // rayon vise à gauche
				stepX = -1;
				sideDistX = (posX - mapX) * deltaDistX;
			} else {
				stepX = 1;
				// triangle semblable -> proportionnel
				sideDistX = (mapX+1.0 - posX) * deltaDistX;
			}
			if (rayDirY<0){
				stepY = -1;
				//idem
				sideDistY = (posY - mapY) * deltaDistY;
			} else {
				stepY = 1;
				sideDistY = (mapY+1.0 - posY) * deltaDistY;
			}

			//DDA
			while (!hit){
				if (sideDistY<sideDistX){ // saut vertical
					side = 1; // NS
					mapY += stepY;
					sideDistY += deltaDistY;

				} else { // saut horizontal
					side = 0; // EW
					mapX += stepX;
					sideDistX += deltaDistX;
				}

				// check collision de mur
				if (map[mapX][mapY]>0){
					hit = 1;
				}

				// calcul distance mur->camera
				if (side==1) { // attaque NS
					perpWallDist = sideDistY - deltaDistY;
				} else {
					perpWallDist = sideDistX - deltaDistX;
				}

			} // fin DDA

			// calculs texturation
			double wallX; // vrai point de hit du mur
			if (side==0) wallX = posY + perpWallDist * rayDirY;
			else 		wallX = posX + perpWallDist * rayDirX;
			wallX -= floor(wallX);

			int texX = floor(wallX * (double)textureWidth); // coordonnée texture à dessiner
			if (rayDirX>0 && side==0) texX = textureWidth - texX - 1;
			if (rayDirY<0 && side==1) texX = textureWidth - texX - 1;

			// dessin
			int wallHeight = (int)(screenHeight / perpWallDist );

			int lineStart = (screenHeight - wallHeight) / 2;   // peut être négatif
			int drawStart = lineStart < 0 ? 0 : lineStart;
			int drawEnd   = (screenHeight + wallHeight) / 2;
			if (drawEnd >= screenHeight) drawEnd = screenHeight - 1;

			double texStep = (double)textureHeight / wallHeight;
			double texPos  = (drawStart - lineStart) * texStep;  // décalage dû au clamp

			for (int y = drawStart; y <= drawEnd; y++) {
				int texY = (int)texPos;
				if (texY > textureHeight - 1) texY = textureHeight - 1;
				texPos += texStep;

				// obtenir le texel et l'assombrir en fonction de dist et side
				Color c = wallTexture[texX + texY * textureWidth];
				double shade = 1.0 + perpWallDist / (double)ambientLight + 1.3*side;
				c.r /= shade; c.g /= shade; c.b /= shade;

				framebuffer[x + y * screenWidth] = c;
			}

			// stockage distance mur pour spritecasting
			ZBuffer[x] = perpWallDist;


	}







		//SPRITECASTING
		double spritesDist[numSprites];
		int spriteOrder[numSprites];
		for (int s=0 ; s<numSprites ; s++){
		    // calcul des distances des sprites
			double dx = sprites[s].posX - posX;
			double dy = sprites[s].posY - posY;
			spritesDist[s] = sqrt(dx*dx + dy*dy);
		}

		for (int s=0 ; s<numSprites ; s++){

		    // coordonnées dans le repère camera
				// transformY : distance perpendiculaire a l'axe camera (profondeur)
				// transformX : distance au centre camera (decalage)
		    double relativeX = sprites[s].posX - posX;
		    double relativeY = sprites[s].posY - posY;
			// multiplier par inv matrice camera
			//  [planeX  dirX]  ->   [dirY     -dirX] * 1/(planeX*dirY-dirX*planeY)
			//  [planeY  dirY]  inv  [-planeY planeX]
			double invDet = 1.0/(planeX*dirY-dirX*planeY);
			double transformX = dirY*relativeX - dirX*relativeY;
			double transformY = -planeY*relativeX + planeX*relativeY;
			transformX *= invDet;
			transformY *= invDet;
			//printf("transformX: %f, transformY: %f\n", transformX, transformY);

			if (transformY<=0) continue; // derrière la camera

			// calcul taille sprite
			int spriteWidth = (int)(screenHeight/transformY);
			int spriteHeight = spriteWidth; // sprite carré

			// rapport profondeur-decalage
			// le sprite est visible si le rapport est entre -1 (max gauche)
			// et 1 (max droite), car FOV de rapport 1
			double ratio = transformX/transformY;
			int screenX = (int)((ratio+1)/2 * screenWidth);

			// bornes du sprite (absolues)
			int startX = screenX - spriteWidth/2;
			int endX   = startX + spriteWidth;
			int startY = (screenHeight - spriteHeight) / 2;
			int endY   = startY + spriteHeight;

			// bornes du sprite (dessin)
			int drawStartX = startX < 0 ? 0 : startX;
			int drawEndX   = endX > screenWidth ? screenWidth : endX;
			int drawStartY = startY < 0 ? 0 : startY;
			int drawEndY   = endY > screenHeight ? screenHeight : endY;

			// shade distance
			double shadeSprite = 1.0 + transformY / (double)ambientLight;

			for (int x = drawStartX; x < drawEndX; x++){
				if (ZBuffer[x]<transformY) continue; // derrière un mur
			    int tx = (x - startX) * textureWidth / spriteWidth;
			    for (int y = drawStartY; y < drawEndY; y++){
			        int ty = (y - startY) * textureHeight / spriteHeight;
					Color texel = sprites[s].texture[tx + ty*textureWidth];
					if (texel.a > 0){
						texel.r/=shadeSprite;texel.g/=shadeSprite;texel.b/=shadeSprite;
						framebuffer[x + y*screenWidth] = texel;
					}
				}
			}
		}











    	UpdateTexture(texture, framebuffer);
    	DrawTexture(texture, 0, 0, WHITE);

    	double rotSpeed = 0.2;
    	double walkSpeed = 0.2;

    	if (IsKeyDown(KEY_UP)){
    		if(map[(int)(posX + dirX * walkSpeed)][(int)(posY)] == false)
    			posX += dirX * walkSpeed;
    		if(map[(int)(posX)][(int)(posY + dirY * walkSpeed)] == false)
    			posY += dirY * walkSpeed;
    	}
    	if (IsKeyDown(KEY_DOWN)){
        	if(map[(int)(posX - dirX * walkSpeed)][(int)(posY)] == false)
        		posX -= dirX * walkSpeed;
        	if(map[(int)(posX)][(int)(posY - dirY * walkSpeed)] == false)
        		posY -= dirY * walkSpeed;
    	}
    	if (IsKeyDown(KEY_RIGHT)){
    		double oldDirX = dirX;
    		dirX = dirX * cos(-rotSpeed) - dirY * sin(-rotSpeed);
    		dirY = oldDirX * sin(-rotSpeed) + dirY * cos(-rotSpeed);
    		double oldPlaneX = planeX;
    		planeX = planeX * cos(-rotSpeed) - planeY * sin(-rotSpeed);
    		planeY = oldPlaneX * sin(-rotSpeed) + planeY * cos(-rotSpeed);
    	}
    	if (IsKeyDown(KEY_LEFT)){
    		double oldDirX = dirX;
    		dirX = dirX * cos(rotSpeed) - dirY * sin(rotSpeed);
    		dirY = oldDirX * sin(rotSpeed) + dirY * cos(rotSpeed);
    		double oldPlaneX = planeX;
    		planeX = planeX * cos(rotSpeed) - planeY * sin(rotSpeed);
    		planeY = oldPlaneX * sin(rotSpeed) + planeY * cos(rotSpeed);
    	}


    	EndDrawing();
    	for (int i=0;i<screenHeight*screenWidth;i++) framebuffer[i]=BLACK;

    }

	CloseWindow();
}








Color* loadTexture(char* filename){

	Image image = LoadImage(filename);
	Color *tex = LoadImageColors(image);
	UnloadImage(image);

	return tex;

}

/*
Sprite* sortSprites(Sprite *_sprites, double *_spritesDist, int _numSprites){
    //select sort
    Sprite sorted[_numSprites];
    double maxDist = 0;
    int maxDistIdx = 0;
    int start=0;
    while(start<_numSprites){
        for (int i=start ; i<_numSprites ; i++){
            if (_spritesDist[i]>maxDist){
                maxDist = _spritesDist[i];
                maxDistIdx = i;
            }
        }
        sorted[start] = _sprites[maxDistIdx];
        start ++;
    }

    return sorted;
}
*/
