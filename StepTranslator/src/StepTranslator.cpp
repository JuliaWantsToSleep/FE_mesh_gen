#include <memory>

#include <BRep_Builder.hxx>
#include <BRepBuilderAPI_NurbsConvert.hxx>
#include <BRepLib_FindSurface.hxx>
#include <GeomConvert.hxx>
#include <Geom_Surface.hxx>
#include <Message_ProgressIndicator.hxx>
#include <STEPControl_Reader.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Solid.hxx>

#include "StepTranslator.hpp"

void StepTranslator::Translate(const std::string& path, std::vector
                               <std::shared_ptr<BSplineSurface>>& surfaces) const
{
    STEPControl_Reader reader;
    IFSelect_ReturnStatus stat = reader.ReadFile(path.c_str());
    if (stat != IFSelect_RetDone)
    {
        reader.PrintCheckLoad(true, IFSelect_ItemsByEntity);
        std::string what = "Error while reading file " + path + "\n";
        throw std::runtime_error(what);
    }
    int roots_num = reader.NbRootsForTransfer();
    for (int i = 1; i <= roots_num; ++i)
    {
        std::cout << "Transferring Root " << i << std::endl;
        reader.TransferRoot(i);
    }

    int shapes_num = reader.NbShapes();
    if (shapes_num == 0)
    {
        std::string what = "STEP file " + path + " has no shapes.\n";
        throw std::runtime_error(what);
    }

    for (int i = 1; i <= roots_num; ++i)
    {
        std::cout << "Transferring Shape " << i << std::endl;
        TopoDS_Shape shape = reader.Shape(i);
        TopExp_Explorer explorer;

        BRep_Builder builder;
        TopoDS_Compound comp;
        builder.MakeCompound(comp);

        for (explorer.Init(shape, TopAbs_SOLID); explorer.More();
             explorer.Next())
        {
            if (!explorer.Current().IsNull())
            {
                builder.Add(comp, explorer.Current());
            }

        }

        for (explorer.Init(shape, TopAbs_SHELL, TopAbs_SOLID); explorer.More();
             explorer.Next())
        {
            if (!explorer.Current().IsNull())
            {
                builder.Add(comp, explorer.Current());
            }
        }
        for (explorer.Init(shape, TopAbs_FACE, TopAbs_SHELL); explorer.More();
             explorer.Next())
        {
            if (!explorer.Current().IsNull())
            {
                builder.Add(comp, explorer.Current());
            }
        }

        for (explorer.Init(shape, TopAbs_WIRE, TopAbs_FACE); explorer.More();
             explorer.Next())
        {
            if (!explorer.Current().IsNull())
            {
                builder.Add(comp, explorer.Current());
            }
        }
        for (explorer.Init(shape, TopAbs_EDGE, TopAbs_WIRE); explorer.More();
             explorer.Next())
        {
            if (!explorer.Current().IsNull())
            {
                builder.Add(comp, explorer.Current());
            }
        }
        for (explorer.Init(shape, TopAbs_VERTEX, TopAbs_EDGE); explorer.More();
             explorer.Next())
        {
            if (!explorer.Current().IsNull())
            {
                builder.Add(comp, explorer.Current());
            }
        }
        if(comp.IsNull())
            std::cout << "WARNING: Could not create compound\n";
        else
        {
            TopoDS_Shape topo_shape(std::move(comp));
            BRepBuilderAPI_NurbsConvert nurbs_conv(topo_shape);
            opencascade::handle<Geom_Surface> geom =
                    BRepLib_FindSurface(nurbs_conv).Surface();
            opencascade::handle<Geom_BSplineSurface> g_bspline_surface =
                    GeomConvert::SurfaceToBSplineSurface(geom);
            surfaces.emplace_back(std::make_shared<BSplineSurface>
                                  (std::move(*g_bspline_surface.get())));
        }
    }
}
