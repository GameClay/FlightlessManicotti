/* -*- Mode: C; tab-width: 3; c-basic-offset: 3; indent-tabs-mode: nil -*- */
/* vim: set filetype=C tabstop=3 softtabstop=3 shiftwidth=3 expandtab: */

/* FlightlessManicotti -- Copyright (C) 2010-2012 GameClay LLC
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <FlightlessManicotti/math/math.h>

/* Torus knot generation (http://sol.gfxile.net/code.html)
   written by Jari Komppa aka Sol / Trauma
   Based on:
   http://www.blackpawn.com/texts/pqtorus/default.html */

void torusknot_sizes(int aSteps, int aFacets, size_t* nVerts, size_t* nIdx)
{
   *nIdx = (aSteps + 1) * aFacets * 2;
   *nVerts = ((aSteps + 1) * (aFacets + 1) + 1);
}

void generate_torusknot(int aSteps,           /* in: Number of steps in the torus knot */
                        int aFacets,          /* in: Number of facets */
                        float aScale,         /* in: Scale of the knot */
                        float aThickness,     /* in: Thickness of the knot */
                        float aClumps,        /* in: Number of clumps in the knot */
                        float aClumpOffset,   /* in: Offset of the clump (in 0..2pi) */
                        float aClumpScale,    /* in: Scale of a clump */
                        float aUScale,        /* in: U coordinate scale */
                        float aVScale,        /* in: V coordinate scale */
                        float aP,             /* in: P parameter of the knot */
                        float aQ,             /* in: Q parameter of the knot */

                        float* spectrum,      /* in: values modifying scale */
                        size_t nSpectrum,     /* in: number of values in spectrum */

                        float** verts,        /* out: vertices */
                        float** normals,      /* out: normals */
                        float** uvs,          /* out: tex coords */
                        uint16_t** indices,   /* out: indices */
                        size_t* nVerts,       /* in/out: number of verts/normals/uvs */
                        size_t* nIdx)         /* in/out: number of indices */
{
   int i, j;

   float *vtx      = *verts;
   float *normal   = *normals;
   float *texcoord = (uvs == NULL ? NULL : *uvs);
   uint16_t *idx   = (indices == NULL ? NULL : *indices);
   size_t nindices = (aSteps + 1) * aFacets * 2;
   size_t nvertices = ((aSteps + 1) * (aFacets + 1) + 1);

   if(nindices < *nIdx || nvertices < *nVerts)
   {
      *nIdx = 0;
      *nVerts = 0;
      return;
   }

   aThickness *= aScale;

   *nIdx = (aSteps + 1) * aFacets * 2;
   *nVerts = ((aSteps + 1) * (aFacets + 1) + 1);

   if(idx != NULL)
   {
      for(j = 0; j < aFacets; j++)
      {
         for(i = 0; i < aSteps + 1; i++)
         {
            idx[i * 2 + 0 + j * (aSteps + 1) * 2] = ((j + 1) + i * (aFacets + 1));
            idx[i * 2 + 1 + j * (aSteps + 1) * 2] = (j + i * (aFacets + 1));
         }
      }
   }

   for(i = 0; i < aSteps; i++)
   {
      float l;
      float centerpoint[3];
      float nextpoint[3];
      float T[3];
      float N[3];
      float B[3];
      float Pp = aP * i * KL_2PI_F / aSteps;
      float Qp = aQ * i * KL_2PI_F / aSteps;
      float r;

      r = (.5f * (2 + kl_sin(Qp))) * aScale;

      centerpoint[0] = r * kl_cos(Pp);
      centerpoint[1] = r * kl_cos(Qp);
      centerpoint[2] = r * kl_sin(Pp);

      Pp = aP * (i + 1) * KL_2PI_F / aSteps;
      Qp = aQ * (i + 1) * KL_2PI_F / aSteps;
      r = (.5f * (2 + kl_sin(Qp))) * aScale;
      nextpoint[0] = r * kl_cos(Pp);
      nextpoint[1] = r * kl_cos(Qp);
      nextpoint[2] = r * kl_sin(Pp);

      T[0] = nextpoint[0] - centerpoint[0];
      T[1] = nextpoint[1] - centerpoint[1];
      T[2] = nextpoint[2] - centerpoint[2];

      N[0] = nextpoint[0] + centerpoint[0];
      N[1] = nextpoint[1] + centerpoint[1];
      N[2] = nextpoint[2] + centerpoint[2];

      B[0] = T[1]*N[2] - T[2]*N[1];
      B[1] = T[2]*N[0] - T[0]*N[2];
      B[2] = T[0]*N[1] - T[1]*N[0];

      N[0] = B[1]*T[2] - B[2]*T[1];
      N[1] = B[2]*T[0] - B[0]*T[2];
      N[2] = B[0]*T[1] - B[1]*T[0];

      l = (float)sqrt(B[0] * B[0] + B[1] * B[1] + B[2] * B[2]);
      B[0] /= l;
      B[1] /= l;
      B[2] /= l;

      l = (float)sqrt(N[0] * N[0] + N[1] * N[1] + N[2] * N[2]);
      N[0] /= l;
      N[1] /= l;
      N[2] /= l;

      for(j = 0; j < aFacets; j++)
      {

         float pointx, pointy, l;
         float facetThickness = aThickness;

         if(spectrum != NULL)
         {
            int idx = floor(((float)i / aSteps) * nSpectrum);
            facetThickness += spectrum[idx];
         }

         pointx = kl_sin(j * KL_2PI_F / aFacets) * facetThickness * ((kl_sin(aClumpOffset + aClumps * i * KL_2PI_F / aSteps) * aClumpScale) + 1);
         pointy = kl_cos(j * KL_2PI_F / aFacets) * facetThickness * ((kl_cos(aClumpOffset + aClumps * i * KL_2PI_F / aSteps) * aClumpScale) + 1);

         vtx[i * (aFacets + 1) * 3 + j * 3 + 0] = N[0] * pointx + B[0] * pointy + centerpoint[0];
         vtx[i * (aFacets + 1) * 3 + j * 3 + 1] = N[1] * pointx + B[1] * pointy + centerpoint[1];
         vtx[i * (aFacets + 1) * 3 + j * 3 + 2] = N[2] * pointx + B[2] * pointy + centerpoint[2];

         normal[i * (aFacets + 1) * 3 + j * 3 + 0] = vtx[i * (aFacets + 1) * 3 + j * 3 + 0] - centerpoint[0];
         normal[i * (aFacets + 1) * 3 + j * 3 + 1] = vtx[i * (aFacets + 1) * 3 + j * 3 + 1] - centerpoint[1];
         normal[i * (aFacets + 1) * 3 + j * 3 + 2] = vtx[i * (aFacets + 1) * 3 + j * 3 + 2] - centerpoint[2];

         l = (float)sqrt(normal[i * (aFacets + 1) * 3 + j * 3 + 0] * normal[i * (aFacets + 1) * 3 + j * 3 + 0] +
                     normal[i * (aFacets + 1) * 3 + j * 3 + 1] * normal[i * (aFacets + 1) * 3 + j * 3 + 1] +
                     normal[i * (aFacets + 1) * 3 + j * 3 + 2] * normal[i * (aFacets + 1) * 3 + j * 3 + 2]);
         
         normal[i * (aFacets + 1) * 3 + j * 3 + 0] /= l;
         normal[i * (aFacets + 1) * 3 + j * 3 + 1] /= l;
         normal[i * (aFacets + 1) * 3 + j * 3 + 2] /= l;

         if(texcoord != NULL)
         {
            texcoord[i * (aFacets + 1) * 2 + j * 2 + 0] = ((float)j / aFacets) * aUScale;
            texcoord[i * (aFacets + 1) * 2 + j * 2 + 1] = ((float)i / aSteps) * aVScale;
         }
      }

      /* create duplicate vertex for sideways wrapping
         otherwise identical to first vertex in the 'ring' except for the U coordinate */
      vtx[i * (aFacets + 1) * 3 + aFacets * 3 + 0] = vtx[i * (aFacets + 1) * 3 + 0];
      vtx[i * (aFacets + 1) * 3 + aFacets * 3 + 1] = vtx[i * (aFacets + 1) * 3 + 1];
      vtx[i * (aFacets + 1) * 3 + aFacets * 3 + 2] = vtx[i * (aFacets + 1) * 3 + 2];

      normal[i * (aFacets + 1) * 3 + aFacets * 3 + 0] = normal[i * (aFacets + 1) * 3 + 0];
      normal[i * (aFacets + 1) * 3 + aFacets * 3 + 1] = normal[i * (aFacets + 1) * 3 + 1];
      normal[i * (aFacets + 1) * 3 + aFacets * 3 + 2] = normal[i * (aFacets + 1) * 3 + 2];

      if(texcoord != NULL)
      {
         texcoord[i * (aFacets + 1) * 2 + aFacets * 2 + 0] = aUScale;
         texcoord[i * (aFacets + 1) * 2 + aFacets * 2 + 1] = texcoord[i * (aFacets + 1) * 2 + 1];
      }
   }

   /* create duplicate ring of vertices for longways wrapping
      otherwise identical to first 'ring' in the knot except for the V coordinate */
   for(j = 0; j < aFacets; j++)
   {
      vtx[aSteps * (aFacets + 1) * 3 + j * 3 + 0] = vtx[j * 3 + 0];
      vtx[aSteps * (aFacets + 1) * 3 + j * 3 + 1] = vtx[j * 3 + 1];
      vtx[aSteps * (aFacets + 1) * 3 + j * 3 + 2] = vtx[j * 3 + 2];

      normal[aSteps * (aFacets + 1) * 3 + j * 3 + 0] = normal[j * 3 + 0];
      normal[aSteps * (aFacets + 1) * 3 + j * 3 + 1] = normal[j * 3 + 1];
      normal[aSteps * (aFacets + 1) * 3 + j * 3 + 2] = normal[j * 3 + 2];

      if(texcoord != NULL)
      {
         texcoord[aSteps * (aFacets + 1) * 2 + j * 2 + 0] = texcoord[j * 2 + 0];
         texcoord[aSteps * (aFacets + 1) * 2 + j * 2 + 1] = aVScale;
      }
   }

   /* finally, there's one vertex that needs to be duplicated due to both U and V coordinate. */

   vtx[aSteps * (aFacets + 1) * 3 + aFacets * 3 + 0] = vtx[0];
   vtx[aSteps * (aFacets + 1) * 3 + aFacets * 3 + 1] = vtx[1];
   vtx[aSteps * (aFacets + 1) * 3 + aFacets * 3 + 2] = vtx[2];

   normal[aSteps * (aFacets + 1) * 3 + aFacets * 3 + 0] = normal[0];
   normal[aSteps * (aFacets + 1) * 3 + aFacets * 3 + 1] = normal[1];
   normal[aSteps * (aFacets + 1) * 3 + aFacets * 3 + 2] = normal[2];

   if(texcoord != NULL)
   {
      texcoord[aSteps * (aFacets + 1) * 2 + aFacets * 2 + 0] = aUScale;
      texcoord[aSteps * (aFacets + 1) * 2 + aFacets * 2 + 1] = aVScale;
   }
}
