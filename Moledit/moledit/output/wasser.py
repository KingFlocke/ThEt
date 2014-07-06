from paraview.simple import *
paraview.simple._DisableFirstRenderCameraReset()

RenderView1 = GetRenderView()
RenderView1.CameraPosition = [9.0, 27.5, 126.08041089866609]
RenderView1.CameraFocalPoint = [9.0, 27.5, 9.0]
RenderView1.CameraClippingRange = [97.14598463925829, 141.80842813735666]
RenderView1.CameraParallelScale = 30.30264014900352
RenderView1.CenterOfRotation = [9.0, 27.5, 9.0]

wasser_box_vtk = LegacyVTKReader(FileNames=['wasser_box.vtk'], guiName = "wasser_box")
DataRepresentation1 = Show()

wasser_comp1_vtk = LegacyVTKReader(FileNames=['wasser_comp1.vtk'], guiName = "wasser_comp1")
DataRepresentation2 = Show()

wasser_comp2_vtk = LegacyVTKReader(FileNames=['wasser_comp2.vtk'], guiName = "wasser_comp2")
DataRepresentation3 = Show()

SetActiveSource(wasser_comp1_vtk)
Glyph1 = Glyph( GlyphType="Arrow", GlyphTransform="Transform2" )

Glyph1.Scalars = ['POINTS', '']
Glyph1.Vectors = ['POINTS', '']
Glyph1.GlyphTransform = "Transform2"

Glyph1.SetScaleFactor = 1.0
Glyph1.GlyphType = "2D Glyph"

Glyph1.GlyphType.GlyphType = 'Vertex'

DataRepresentation4 = Show()

DataRepresentation4.DiffuseColor = [0.0, 1.0, 0.0]

SetActiveSource(wasser_comp2_vtk)
Glyph1 = Glyph( GlyphType="Arrow", GlyphTransform="Transform2" )

Glyph1.Scalars = ['POINTS', '']
Glyph1.Vectors = ['POINTS', '']
Glyph1.GlyphTransform = "Transform2"

Glyph1.SetScaleFactor = 1.0
Glyph1.GlyphType = "2D Glyph"

Glyph1.GlyphType.GlyphType = 'Vertex'

DataRepresentation5 = Show()

DataRepresentation5.DiffuseColor = [0.0, 0.0, 1.0]

Render()
