from paraview.simple import *
paraview.simple._DisableFirstRenderCameraReset()

RenderView1 = GetRenderView()
RenderView1.CameraPosition = [9.0, 27.5, 126.08041089866609]
RenderView1.CameraFocalPoint = [9.0, 27.5, 9.0]
RenderView1.CameraClippingRange = [97.14598463925829, 141.80842813735666]
RenderView1.CameraParallelScale = 30.30264014900352
RenderView1.CenterOfRotation = [9.0, 27.5, 9.0]

asd_box_vtk = LegacyVTKReader(FileNames=['asd_box.vtk'], guiName = "asd_box")
DataRepresentation1 = Show()

asd_comp1_vtk = LegacyVTKReader(FileNames=['asd_comp1.vtk'], guiName = "asd_comp1")
DataRepresentation2 = Show()

asd_comp2_vtk = LegacyVTKReader(FileNames=['asd_comp2.vtk'], guiName = "asd_comp2")
DataRepresentation3 = Show()

SetActiveSource(asd_comp1_vtk)
Glyph1 = Glyph( GlyphType="Sphere", GlyphTransform="Transform2" )

Glyph1.Scalars = ['POINTS', '']
Glyph1.Vectors = ['POINTS', '']

Glyph1.SetScaleFactor = 1.0

Glyph1.GlyphType.Radius = 0.4

DataRepresentation4 = Show()

DataRepresentation4.DiffuseColor = [0.0, 0.0, 1.0]

SetActiveSource(asd_comp2_vtk)
Glyph2 = Glyph( GlyphType="Sphere", GlyphTransform="Transform2" )

Glyph2.Scalars = ['POINTS', '']
Glyph2.Vectors = ['POINTS', '']

Glyph2.SetScaleFactor = 1.0

Glyph2.GlyphType.Radius = 0.35

DataRepresentation5 = Show()

DataRepresentation5.DiffuseColor = [1.0, 0.0, 0.0]

Render()
