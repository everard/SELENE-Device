// Copyright (c) 2012 Nezametdinov E. Ildus
// Licensed under the MIT License (see LICENSE.txt for details)

#ifndef D3D9_GUI_RENDERER_H
#define D3D9_GUI_RENDERER_H

#include "../../../../../Engine/Core/Macros/Macros.h"
#include "../D3D9Device.h"

namespace selene
{

        // Forward declaration of classes
        class FileManager;
        class Gui;

        // Represents D3D9 GUI renderer.
        class D3d9GuiRenderer
        {
        public:
                D3d9GuiRenderer();
                ~D3d9GuiRenderer();

                // Initializes GUI renderer
                bool initialize(FileManager* fileManager);

                // Destroys GUI renderer
                void destroy();

                // Sets GUI
                void setGui(Gui* gui);

                // Prepares rendering
                bool prepare();

                // Renders GUI frames
                bool renderFrames();

                // Renders GUI text
                bool renderText();

        private:
                LPDIRECT3DVERTEXDECLARATION9 d3dVertexDeclaration_;
                LPDIRECT3DVERTEXBUFFER9 d3dVertexBuffer_;
                LPDIRECT3DTEXTURE9 d3dFontTexture_;
                LPDIRECT3DDEVICE9 d3dDevice_;
                Gui* gui_;

                uint32_t textVertexIndex_;
                uint32_t numFrameFaces_;
                uint32_t numTextFaces_;

                static const uint32_t numVerticesPerElement_;
                static const uint32_t vertexStride_;
                static const uint32_t numVertices_;
                static const uint32_t numFaces_;

        };

}

#endif
