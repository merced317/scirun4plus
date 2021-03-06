/*
   For more information, please see: http://software.sci.utah.edu

   The MIT License

   Copyright (c) 2009 Scientific Computing and Imaging Institute,
   University of Utah.

   
   Permission is hereby granted, free of charge, to any person obtaining a
   copy of this software and associated documentation files (the "Software"),
   to deal in the Software without restriction, including without limitation
   the rights to use, copy, modify, merge, publish, distribute, sublicense,
   and/or sell copies of the Software, and to permit persons to whom the
   Software is furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included
   in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
   OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
   THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
   FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
   DEALINGS IN THE SOFTWARE.
*/


/*
 *  BoxWidget.cc
 *
 *  Written by:
 *   James Purciful
 *   Department of Computer Science
 *   University of Utah
 *   Jan. 1995
 *
 *  Update Log
 *  ~~~~~~ ~~~
 *   7.1.95 -- DWM -- Added aligned member and methods (copied from  BoxWidget)
 */

#include <Dataflow/Widgets/BoxWidget.h>
#include <Dataflow/Constraints/DistanceConstraint.h>
#include <Dataflow/Constraints/PythagorasConstraint.h>
#include <Dataflow/Constraints/RatioConstraint.h>
#include <Core/Geom/GeomCylinder.h>
#include <Core/Geom/GeomSphere.h>

#include <Dataflow/Network/Module.h>

namespace SCIRun {



const Index NumSwitches = 4;
const Index NumSchemes = 7;

enum { ConstRD, ConstDI, ConstIR, ConstRC, ConstDC, ConstIC,
       ConstPythRD, ConstPythDI, ConstPythIR,
       ConstRatioR, ConstRatioD, ConstRatioI, NumCons };
enum { SphereR, SphereL, SphereD, SphereU, SphereI, SphereO,
       SmallSphereIUL, SmallSphereIUR, SmallSphereIDR, SmallSphereIDL,
       SmallSphereOUL, SmallSphereOUR, SmallSphereODR, SmallSphereODL,
       CylIU, CylIR, CylID, CylIL,
       CylMU, CylMR, CylMD, CylML,
       CylOU, CylOR, CylOD, CylOL,
       GeomResizeR, GeomResizeL, GeomResizeD, GeomResizeU,
       GeomResizeI, GeomResizeO,
       SliderCylR, SliderCylD, SliderCylI, NumGeoms };
enum { PickSphR, PickSphL, PickSphD, PickSphU, PickSphI, PickSphO,
       PickCyls,
       PickResizeR, PickResizeL, PickResizeD, PickResizeU,
       PickResizeI, PickResizeO,
       PickSliderR, PickSliderD, PickSliderI, NumPcks };

/***************************************************************************
 * The constructor initializes the widget's constraints, variables,
 *      geometry, picks, materials, modes, switches, and schemes.
 * Variables and constraints are initialized as a function of the
 *      widget_scale.
 * Much of the work is accomplished in the BaseWidget constructor which
 *      includes some consistency checking to ensure full initialization.
 */
BoxWidget::BoxWidget( Module* module, CrowdMonitor* lock, 
		      double widget_scale, bool aligned , bool slideable) :
  BaseWidget(module, lock, "BoxWidget",
	     NumVars, NumCons, NumGeoms, NumPcks, NumMatls,
	     0, NumSwitches, widget_scale),
  is_aligned_(aligned),
  is_slideable_(slideable),
  restrict_translation_xyz_(false),
  restrict_x_(false),
  restrict_y_(false),
  restrict_z_(false),
  restrict_translation_rdi_(false),
  restrict_r_(false),
  restrict_d_(false),
  restrict_i_(false),
  oldrightaxis(1, 0, 0),
  olddownaxis(0, 1, 0),
  oldinaxis(0, 0, 1)
{
  const double INIT = 5.0*widget_scale;
  variables[CenterVar] = new PointVariable("Center", solve, Scheme1, Point(0, 0, 0));
  variables[PointRVar] = new PointVariable("PntR", solve, Scheme1, Point(INIT, 0, 0));
  variables[PointDVar] = new PointVariable("PntD", solve, Scheme2, Point(0, INIT, 0));
  variables[PointIVar] = new PointVariable("PntI", solve, Scheme3, Point(0, 0, INIT));
  variables[DistRVar] = new RealVariable("DISTR", solve, Scheme4, INIT);
  variables[DistDVar] = new RealVariable("DISTD", solve, Scheme5, INIT);
  variables[DistIVar] = new RealVariable("DISTI", solve, Scheme6, INIT);
  variables[HypoRDVar] = new RealVariable("HYPOR", solve, Scheme4, sqrt(2*INIT*INIT));
  variables[HypoDIVar] = new RealVariable("HYPOD", solve, Scheme5, sqrt(2*INIT*INIT));
  variables[HypoIRVar] = new RealVariable("HYPOI", solve, Scheme6, sqrt(2*INIT*INIT));
  variables[SDistRVar] = new RealVariable("SDistR", solve, Scheme7, INIT/2.0);
  variables[SDistDVar] = new RealVariable("SDistD", solve, Scheme7, INIT/2.0);
  variables[SDistIVar] = new RealVariable("SDistI", solve, Scheme7, INIT/2.0);
  variables[RatioRVar] = new RealVariable("RatioR", solve, Scheme1, 0.5);
  variables[RatioDVar] = new RealVariable("RatioD", solve, Scheme1, 0.5);
  variables[RatioIVar] = new RealVariable("RatioI", solve, Scheme1, 0.5);

  constraints[ConstRatioR] = new RatioConstraint("ConstRatioR",
						    NumSchemes,
						    variables[SDistRVar],
						    variables[DistRVar],
						    variables[RatioRVar]);
  constraints[ConstRatioR]->VarChoices(Scheme1, 0, 0, 0);
  constraints[ConstRatioR]->VarChoices(Scheme2, 0, 0, 0);
  constraints[ConstRatioR]->VarChoices(Scheme3, 0, 0, 0);
  constraints[ConstRatioR]->VarChoices(Scheme4, 0, 0, 0);
  constraints[ConstRatioR]->VarChoices(Scheme5, 0, 0, 0);
  constraints[ConstRatioR]->VarChoices(Scheme6, 0, 0, 0);
  constraints[ConstRatioR]->VarChoices(Scheme7, 2, 2, 2);
  constraints[ConstRatioR]->Priorities(P_Highest, P_Highest, P_Highest);
  constraints[ConstRatioD] = new RatioConstraint("ConstRatioD",
						    NumSchemes,
						    variables[SDistDVar],
						    variables[DistDVar],
						    variables[RatioDVar]);
  constraints[ConstRatioD]->VarChoices(Scheme1, 0, 0, 0);
  constraints[ConstRatioD]->VarChoices(Scheme2, 0, 0, 0);
  constraints[ConstRatioD]->VarChoices(Scheme3, 0, 0, 0);
  constraints[ConstRatioD]->VarChoices(Scheme4, 0, 0, 0);
  constraints[ConstRatioD]->VarChoices(Scheme5, 0, 0, 0);
  constraints[ConstRatioD]->VarChoices(Scheme6, 0, 0, 0);
  constraints[ConstRatioD]->VarChoices(Scheme7, 2, 2, 2);
  constraints[ConstRatioD]->Priorities(P_Highest, P_Highest, P_Highest);
  constraints[ConstRatioI] = new RatioConstraint("ConstRatioI",
						    NumSchemes,
						    variables[SDistIVar],
						    variables[DistIVar],
						    variables[RatioIVar]);
  constraints[ConstRatioI]->VarChoices(Scheme1, 0, 0, 0);
  constraints[ConstRatioI]->VarChoices(Scheme2, 0, 0, 0);
  constraints[ConstRatioI]->VarChoices(Scheme3, 0, 0, 0);
  constraints[ConstRatioI]->VarChoices(Scheme4, 0, 0, 0);
  constraints[ConstRatioI]->VarChoices(Scheme5, 0, 0, 0);
  constraints[ConstRatioI]->VarChoices(Scheme6, 0, 0, 0);
  constraints[ConstRatioI]->VarChoices(Scheme7, 2, 2, 2);
  constraints[ConstRatioI]->Priorities(P_Highest, P_Highest, P_Highest);
  constraints[ConstRD] = new DistanceConstraint("ConstRD",
						   NumSchemes,
						   variables[PointRVar],
						   variables[PointDVar],
						   variables[HypoRDVar]);
  constraints[ConstRD]->VarChoices(Scheme1, 1, 1, 1);
  constraints[ConstRD]->VarChoices(Scheme2, 0, 0, 0);
  constraints[ConstRD]->VarChoices(Scheme3, 0, 0, 0);
  constraints[ConstRD]->VarChoices(Scheme4, 2, 2, 1);
  constraints[ConstRD]->VarChoices(Scheme5, 2, 2, 0);
  constraints[ConstRD]->VarChoices(Scheme6, 2, 2, 0);
  constraints[ConstRD]->VarChoices(Scheme7, 1, 0, 1);
  constraints[ConstRD]->Priorities(P_Default, P_Default, P_Default);
  constraints[ConstPythRD] = new PythagorasConstraint("ConstPythRD",
							 NumSchemes,
							 variables[DistRVar],
							 variables[DistDVar],
							 variables[HypoRDVar]);
  constraints[ConstPythRD]->VarChoices(Scheme1, 1, 0, 1);
  constraints[ConstPythRD]->VarChoices(Scheme2, 1, 0, 0);
  constraints[ConstPythRD]->VarChoices(Scheme3, 1, 0, 0);
  constraints[ConstPythRD]->VarChoices(Scheme4, 2, 2, 1);
  constraints[ConstPythRD]->VarChoices(Scheme5, 2, 2, 0);
  constraints[ConstPythRD]->VarChoices(Scheme6, 2, 2, 0);
  constraints[ConstPythRD]->VarChoices(Scheme7, 1, 0, 1);
  constraints[ConstPythRD]->Priorities(P_Highest, P_Highest, P_Highest);
  constraints[ConstDI] = new DistanceConstraint("ConstDI",
						   NumSchemes,
						   variables[PointDVar],
						   variables[PointIVar],
						   variables[HypoDIVar]);
  constraints[ConstDI]->VarChoices(Scheme1, 0, 0, 0);
  constraints[ConstDI]->VarChoices(Scheme2, 1, 1, 1);
  constraints[ConstDI]->VarChoices(Scheme3, 0, 0, 0);
  constraints[ConstDI]->VarChoices(Scheme4, 2, 2, 1);
  constraints[ConstDI]->VarChoices(Scheme5, 2, 2, 0);
  constraints[ConstDI]->VarChoices(Scheme6, 2, 2, 0);
  constraints[ConstDI]->VarChoices(Scheme7, 1, 0, 1);
  constraints[ConstDI]->Priorities(P_Default, P_Default, P_Default);
  constraints[ConstPythDI] = new PythagorasConstraint("ConstPythDI",
							 NumSchemes,
							 variables[DistDVar],
							 variables[DistIVar],
							 variables[HypoDIVar]);
  constraints[ConstPythDI]->VarChoices(Scheme1, 1, 0, 1);
  constraints[ConstPythDI]->VarChoices(Scheme2, 1, 0, 0);
  constraints[ConstPythDI]->VarChoices(Scheme3, 1, 0, 0);
  constraints[ConstPythDI]->VarChoices(Scheme4, 2, 2, 1);
  constraints[ConstPythDI]->VarChoices(Scheme5, 2, 2, 0);
  constraints[ConstPythDI]->VarChoices(Scheme6, 2, 2, 0);
  constraints[ConstPythDI]->VarChoices(Scheme7, 1, 0, 1);
  constraints[ConstPythDI]->Priorities(P_Highest, P_Highest, P_Highest);
  constraints[ConstIR] = new DistanceConstraint("ConstIR",
						   NumSchemes,
						   variables[PointIVar],
						   variables[PointRVar],
						   variables[HypoIRVar]);
  constraints[ConstIR]->VarChoices(Scheme1, 0, 0, 0);
  constraints[ConstIR]->VarChoices(Scheme2, 0, 0, 0);
  constraints[ConstIR]->VarChoices(Scheme3, 1, 1, 1);
  constraints[ConstIR]->VarChoices(Scheme4, 2, 2, 1);
  constraints[ConstIR]->VarChoices(Scheme5, 2, 2, 0);
  constraints[ConstIR]->VarChoices(Scheme6, 2, 2, 0);
  constraints[ConstIR]->VarChoices(Scheme7, 1, 0, 1);
  constraints[ConstIR]->Priorities(P_Default, P_Default, P_Default);
  constraints[ConstPythIR] = new PythagorasConstraint("ConstPythIR",
							 NumSchemes,
							 variables[DistIVar],
							 variables[DistRVar],
							 variables[HypoIRVar]);
  constraints[ConstPythIR]->VarChoices(Scheme1, 1, 0, 1);
  constraints[ConstPythIR]->VarChoices(Scheme2, 1, 0, 0);
  constraints[ConstPythIR]->VarChoices(Scheme3, 1, 0, 0);
  constraints[ConstPythIR]->VarChoices(Scheme4, 2, 2, 1);
  constraints[ConstPythIR]->VarChoices(Scheme5, 2, 2, 0);
  constraints[ConstPythIR]->VarChoices(Scheme6, 2, 2, 0);
  constraints[ConstPythIR]->VarChoices(Scheme7, 1, 0, 1);
  constraints[ConstPythIR]->Priorities(P_Highest, P_Highest, P_Highest);
  constraints[ConstRC] = new DistanceConstraint("ConstRC",
						   NumSchemes,
						   variables[PointRVar],
						   variables[CenterVar],
						   variables[DistRVar]);
  constraints[ConstRC]->VarChoices(Scheme1, 0, 0, 0);
  constraints[ConstRC]->VarChoices(Scheme2, 0, 0, 0);
  constraints[ConstRC]->VarChoices(Scheme3, 0, 0, 0);
  constraints[ConstRC]->VarChoices(Scheme4, 2, 2, 2);
  constraints[ConstRC]->VarChoices(Scheme5, 0, 0, 0);
  constraints[ConstRC]->VarChoices(Scheme6, 0, 0, 0);
  constraints[ConstRC]->VarChoices(Scheme7, 1, 0, 1);
  constraints[ConstRC]->Priorities(P_Highest, P_Highest, P_Default);
  constraints[ConstDC] = new DistanceConstraint("ConstDC",
						   NumSchemes,
						   variables[PointDVar],
						   variables[CenterVar],
						   variables[DistDVar]);
  constraints[ConstDC]->VarChoices(Scheme1, 0, 0, 0);
  constraints[ConstDC]->VarChoices(Scheme2, 0, 0, 0);
  constraints[ConstDC]->VarChoices(Scheme3, 0, 0, 0);
  constraints[ConstDC]->VarChoices(Scheme4, 0, 0, 0);
  constraints[ConstDC]->VarChoices(Scheme5, 2, 2, 2);
  constraints[ConstDC]->VarChoices(Scheme6, 0, 0, 0);
  constraints[ConstDC]->VarChoices(Scheme7, 1, 0, 1);
  constraints[ConstDC]->Priorities(P_Highest, P_Highest, P_Default);
  constraints[ConstIC] = new DistanceConstraint("ConstIC",
						   NumSchemes,
						   variables[PointIVar],
						   variables[CenterVar],
						   variables[DistIVar]);
  constraints[ConstIC]->VarChoices(Scheme1, 0, 0, 0);
  constraints[ConstIC]->VarChoices(Scheme2, 0, 0, 0);
  constraints[ConstIC]->VarChoices(Scheme3, 0, 0, 0);
  constraints[ConstIC]->VarChoices(Scheme4, 0, 0, 0);
  constraints[ConstIC]->VarChoices(Scheme5, 0, 0, 0);
  constraints[ConstIC]->VarChoices(Scheme6, 2, 2, 2);
  constraints[ConstIC]->VarChoices(Scheme7, 1, 0, 1);
  constraints[ConstIC]->Priorities(P_Highest, P_Highest, P_Default);

  Index geom, pick;
  GeomGroup* cyls = new GeomGroup;
  for (geom = SmallSphereIUL; geom <= SmallSphereODL; geom++)
  {
    geometries[geom] = new GeomSphere;
    cyls->add(geometries[geom]);
  }
  for (geom = CylIU; geom <= CylOL; geom++)
  {
    geometries[geom] = new GeomCylinder;
    cyls->add(geometries[geom]);
  }
  picks_[PickCyls] = new GeomPick(cyls, this, PickCyls);
  picks(PickCyls)->set_highlight(DefaultHighlightMaterial);
  materials[EdgeMatl] = new GeomMaterial(picks_[PickCyls], DefaultEdgeMaterial);
  CreateModeSwitch(0, materials[EdgeMatl]);

  GeomGroup* pts = new GeomGroup;
  for (geom = SphereR, pick = PickSphR;
       geom <= SphereO; geom++, pick++)
  {
    geometries[geom] = new GeomSphere;
    picks_[pick] = new GeomPick(geometries[geom], this, pick);
    picks(pick)->set_highlight(DefaultHighlightMaterial);
    pts->add(picks_[pick]);
  }
  materials[PointMatl] = new GeomMaterial(pts, DefaultPointMaterial);
  CreateModeSwitch(1, materials[PointMatl]);
   
  GeomGroup* resizes = new GeomGroup;
  for (geom = GeomResizeR, pick = PickResizeR;
       geom <= GeomResizeO; geom++, pick++)
  {
    geometries[geom] = new GeomCappedCylinder;
    picks_[pick] = new GeomPick(geometries[geom], this, pick);
    picks(pick)->set_highlight(DefaultHighlightMaterial);
    resizes->add(picks_[pick]);
  }
  materials[ResizeMatl] = new GeomMaterial(resizes, DefaultResizeMaterial);
  CreateModeSwitch(2, materials[ResizeMatl]);

  GeomGroup* sliders = new GeomGroup;
  geometries[SliderCylR] = new GeomCappedCylinder;
  picks_[PickSliderR] = new GeomPick(geometries[SliderCylR], this, PickSliderR);
  picks(PickSliderR)->set_highlight(DefaultHighlightMaterial);
  sliders->add(picks_[PickSliderR]);
  geometries[SliderCylD] = new GeomCappedCylinder;
  picks_[PickSliderD] = new GeomPick(geometries[SliderCylD], this, PickSliderD);
  picks(PickSliderD)->set_highlight(DefaultHighlightMaterial);
  sliders->add(picks_[PickSliderD]);
  geometries[SliderCylI] = new GeomCappedCylinder;
  picks_[PickSliderI] = new GeomPick(geometries[SliderCylI], this, PickSliderI);
  picks(PickSliderI)->set_highlight(DefaultHighlightMaterial);
  sliders->add(picks_[PickSliderI]);
  materials[SliderMatl] = new GeomMaterial(sliders, DefaultSliderMaterial);
  CreateModeSwitch(3, materials[SliderMatl]);

  // Switch0 are the bars
  // Switch1 are the rotation points
  // Switch2 are the resize cylinders
  // Switch3 are the sliders
  if (is_aligned_)
  {
    if (is_slideable_)
    {
      SetNumModes(4);
      SetMode(Mode0, Switch0|Switch2|Switch3);
      SetMode(Mode1, Switch0|Switch2);
      SetMode(Mode3, Switch0);
      SetMode(Mode4, Switch0|Switch3);
    }
    else
    {
      SetNumModes(2);
      SetMode(Mode0, Switch0|Switch2);
      SetMode(Mode1, Switch0);
    }
  }
  else
  {
    if (is_slideable_)
    {
      SetNumModes(7);
      SetMode(Mode0, Switch0|Switch1|Switch2|Switch3);
      SetMode(Mode1, Switch0|Switch1|Switch2);
      SetMode(Mode2, Switch0|Switch1);
      SetMode(Mode3, Switch0);
      SetMode(Mode4, Switch0|Switch1|Switch3);
      SetMode(Mode5, Switch0|Switch2|Switch3);
      SetMode(Mode6, Switch0|Switch2);
    }
    else
    {
      SetNumModes(4);
      SetMode(Mode0, Switch0|Switch1|Switch2);
      SetMode(Mode1, Switch0|Switch1);
      SetMode(Mode2, Switch0);
      SetMode(Mode3, Switch0|Switch2);
    }
  }

  FinishWidget();
}


/***************************************************************************
 * The destructor frees the widget's allocated structures.
 * The BaseWidget's destructor frees the widget's constraints, variables,
 *      geometry, picks, materials, modes, switches, and schemes.
 * Therefore, most widgets' destructors will not need to do anything.
 */
BoxWidget::~BoxWidget()
{
}


/***************************************************************************
 * The widget's redraw method changes widget geometry to reflect the
 *      widget's variable values and its widget_scale.
 * Geometry should only be changed if the mode_switch that displays
 *      that geometry is active.
 * Redraw should also set the principal directions for all picks.
 * Redraw should never be called directly; the BaseWidget execute method
 *      calls redraw after establishing the appropriate locks.
 */
void
BoxWidget::redraw()
{
  const double sphererad(widget_scale_);
  const double resizerad(0.5*widget_scale_);
  const double cylinderrad(0.5*widget_scale_);
  Vector Right(GetRightAxis()*variables[DistRVar]->real());
  Vector Down(GetDownAxis()*variables[DistDVar]->real());
  Vector In(GetInAxis()*variables[DistIVar]->real());
  Point Center(variables[CenterVar]->point());
  Point IUL(Center-Right-Down+In);
  Point IUR(Center+Right-Down+In);
  Point IDR(Center+Right+Down+In);
  Point IDL(Center-Right+Down+In);
  Point OUL(Center-Right-Down-In);
  Point OUR(Center+Right-Down-In);
  Point ODR(Center+Right+Down-In);
  Point ODL(Center-Right+Down-In);
  Point U(Center-Down);
  Point R(Center+Right);
  Point D(Center+Down);
  Point L(Center-Right);
  Point I(Center+In);
  Point O(Center-In);
  Vector X(1.0,0.0,0.0);
  Vector Y(0.0,1.0,0.0);
  Vector Z(0.0,0.0,1.0);
   
  // draw the edges
  if (mode_switches[0]->get_state())
  {
    geometry<GeomCylinder*>(CylIU)->move(IUL, IUR, cylinderrad);
    geometry<GeomCylinder*>(CylIR)->move(IUR, IDR, cylinderrad);
    geometry<GeomCylinder*>(CylID)->move(IDR, IDL, cylinderrad);
    geometry<GeomCylinder*>(CylIL)->move(IDL, IUL, cylinderrad);
    geometry<GeomCylinder*>(CylMU)->move(IUL, OUL, cylinderrad);
    geometry<GeomCylinder*>(CylMR)->move(IUR, OUR, cylinderrad);
    geometry<GeomCylinder*>(CylMD)->move(IDR, ODR, cylinderrad);
    geometry<GeomCylinder*>(CylML)->move(IDL, ODL, cylinderrad);
    geometry<GeomCylinder*>(CylOU)->move(OUL, OUR, cylinderrad);
    geometry<GeomCylinder*>(CylOR)->move(OUR, ODR, cylinderrad);
    geometry<GeomCylinder*>(CylOD)->move(ODR, ODL, cylinderrad);
    geometry<GeomCylinder*>(CylOL)->move(ODL, OUL, cylinderrad);
    geometry<GeomSphere*>(SmallSphereIUL)->move(IUL, cylinderrad);
    geometry<GeomSphere*>(SmallSphereIUR)->move(IUR, cylinderrad);
    geometry<GeomSphere*>(SmallSphereIDR)->move(IDR, cylinderrad);
    geometry<GeomSphere*>(SmallSphereIDL)->move(IDL, cylinderrad);
    geometry<GeomSphere*>(SmallSphereOUL)->move(OUL, cylinderrad);
    geometry<GeomSphere*>(SmallSphereOUR)->move(OUR, cylinderrad);
    geometry<GeomSphere*>(SmallSphereODR)->move(ODR, cylinderrad);
    geometry<GeomSphere*>(SmallSphereODL)->move(ODL, cylinderrad);
  }

  // draw the rotating points
  if (mode_switches[1]->get_state())
  {
    geometry<GeomSphere*>(SphereR)->move(R, sphererad);
    geometry<GeomSphere*>(SphereL)->move(L, sphererad);
    geometry<GeomSphere*>(SphereD)->move(D, sphererad);
    geometry<GeomSphere*>(SphereU)->move(U, sphererad);
    geometry<GeomSphere*>(SphereI)->move(I, sphererad);
    geometry<GeomSphere*>(SphereO)->move(O, sphererad);
  }

  // draw the resizing cylinders
  if (mode_switches[2]->get_state())
  {
    const Vector resizeR(GetRightAxis()*1.5*widget_scale_);
    const Vector resizeD(GetDownAxis()*1.5*widget_scale_);
    const Vector resizeI(GetInAxis()*1.5*widget_scale_);
      
    geometry<GeomCappedCylinder*>(GeomResizeR)->move(R, R + resizeR, resizerad);
    geometry<GeomCappedCylinder*>(GeomResizeL)->move(L, L - resizeR, resizerad);
    geometry<GeomCappedCylinder*>(GeomResizeD)->move(D, D + resizeD, resizerad);
    geometry<GeomCappedCylinder*>(GeomResizeU)->move(U, U - resizeD, resizerad);
    geometry<GeomCappedCylinder*>(GeomResizeI)->move(I, I + resizeI, resizerad);
    geometry<GeomCappedCylinder*>(GeomResizeO)->move(O, O - resizeI, resizerad);
  }

  // draw the sliders
  if (mode_switches[3]->get_state())
  {
    Point SliderR(OUL+GetRightAxis()*variables[SDistRVar]->real()*2.0);
    Point SliderD(OUL+GetDownAxis()*variables[SDistDVar]->real()*2.0);
    Point SliderI(OUL+GetInAxis()*variables[SDistIVar]->real()*2.0);
    geometry<GeomCappedCylinder*>(SliderCylR)->move(SliderR - (GetRightAxis() * 0.3 * widget_scale_),
						    SliderR + (GetRightAxis() * 0.3 * widget_scale_),
						    1.1*widget_scale_);
    geometry<GeomCappedCylinder*>(SliderCylD)->move(SliderD - (GetDownAxis() * 0.3 * widget_scale_),
						    SliderD + (GetDownAxis() * 0.3 * widget_scale_),
						    1.1*widget_scale_);
    geometry<GeomCappedCylinder*>(SliderCylI)->move(SliderI - (GetInAxis() * 0.3 * widget_scale_),
						    SliderI + (GetInAxis() * 0.3 * widget_scale_),
						    1.1*widget_scale_);
  }

  Right.safe_normalize();
  Down.safe_normalize();
  In.safe_normalize();
  for (Index geom = 0; geom < NumPcks; geom++)
  {
    if ((geom == PickResizeU) || (geom == PickResizeD) || (geom == PickSliderD))
    {
      picks(geom)->set_principal(Down);
    }
    else if ((geom == PickResizeL) || (geom == PickResizeR) || (geom == PickSliderR))
    {
      picks(geom)->set_principal(Right);
    }
    else if ((geom == PickResizeO) || (geom == PickResizeI) || (geom == PickSliderI))
    {
      picks(geom)->set_principal(In);
    }
    else if ((geom == PickSphL) || (geom == PickSphR))
    {
      picks(geom)->set_principal(X, Y, Z);
    }
    else if ((geom == PickSphU) || (geom == PickSphD))
    {
      picks(geom)->set_principal(X, Y, Z);
    }
    else if ((geom == PickSphO) || (geom == PickSphI))
    {
      picks(geom)->set_principal(X, Y, Z);
    }
    else
    {
      if (restrict_translation_xyz_)
      {
        if (restrict_x_ && restrict_y_ && restrict_z_)
          picks(geom)->set_principal();
        else if (restrict_x_ && restrict_y_)
          picks(geom)->set_principal(Z);
        else if (restrict_x_ && restrict_z_)
          picks(geom)->set_principal(Y);
        else if (restrict_y_ && restrict_z_)
          picks(geom)->set_principal(X);
        else if (restrict_x_)
          picks(geom)->set_principal(Y,Z);
        else if (restrict_y_)
          picks(geom)->set_principal(X,Z);
        else if (restrict_z_)
          picks(geom)->set_principal(X,Y);
      }
      else if(restrict_translation_rdi_)
      {
        if (restrict_r_ && restrict_d_ && restrict_i_)
          picks(geom)->set_principal();
        else if (restrict_r_ && restrict_d_)
          picks(geom)->set_principal(In);
        else if (restrict_r_ && restrict_i_)
          picks(geom)->set_principal(Down);
        else if (restrict_d_ && restrict_i_)
          picks(geom)->set_principal(Right);
        else if (restrict_r_)
          picks(geom)->set_principal(Down,In);
        else if (restrict_d_)
          picks(geom)->set_principal(Right,In);
        else if (restrict_i_)
          picks(geom)->set_principal(Right,Down);
      }
      else
      {
        picks(geom)->set_principal(Right, Down, In);
      }
    }
  }

}

// if rotating, save the start position of the selected widget 
void
BoxWidget::geom_pick( GeomPickHandle p, ViewWindow *w,
		      int pick, const BState&state )
{
  BaseWidget::geom_pick(p, w, pick, state);

  pick_centervar_ = variables[CenterVar]->point();
  pick_pointdvar_ = variables[PointDVar]->point();
  pick_pointrvar_ = variables[PointRVar]->point();
  pick_pointivar_ = variables[PointIVar]->point();

  const Point c2 = (variables[CenterVar]->point().vector()*2).point();
  Point start_point;
  Point norm1, norm2;
  switch(pick)
  {
  case PickSphU:
    start_point = (c2 - pick_pointdvar_).point();
    norm1 = pick_pointrvar_;
    norm2 = pick_pointivar_;
    break;
  case PickSphD:
    start_point = pick_pointdvar_;
    norm1 = pick_pointrvar_;
    norm2 = pick_pointivar_;
    break;
  case PickSphL:
    start_point = (c2 - pick_pointrvar_).point();
    norm1 = pick_pointdvar_;
    norm2 = pick_pointivar_;
    break;
  case PickSphR:
    start_point = pick_pointrvar_;
    norm1 = pick_pointdvar_;
    norm2 = pick_pointivar_;
    break;
  case PickSphO:
    start_point = (c2 - pick_pointivar_).point();
    norm1 = pick_pointdvar_;
    norm2 = pick_pointrvar_;
    break;
  case PickSphI:
    start_point = pick_pointivar_;
    norm1 = pick_pointdvar_;
    norm2 = pick_pointrvar_;
    break;
  default:
    return;
  }
  
  Point c1 = variables[CenterVar]->point();
  rot_start_ray_ = start_point - c1;
  
  Vector vec = Cross(norm1-c1,norm2-c1);
  vec *= 1/Sqrt(vec.length());
  if (Dot(vec,rot_start_ray_) < 0.0) vec = -vec;
  if (rot_start_ray_.length() < vec.length() ) rot_start_ray_ = vec;
}


/***************************************************************************
 * The widget's geom_moved method receives geometry move requests from
 *      the widget's picks.  The widget's variables must be altered to
 *      reflect these changes based upon which pick made the request.
 * No more than one variable should be Set since this triggers solution of
 *      the constraints--multiple Sets could lead to inconsistencies.
 *      The constraint system only requires that a variable be Set if the
 *      change would cause a constraint to be invalid.  For example, if
 *      all PointVariables are moved by the same delta, then no Set is
 *      required.
 * The last line of the widget's geom_moved method should call the
 *      BaseWidget execute method (which calls the redraw method).
 */
void
BoxWidget::geom_moved( GeomPickHandle, int axis, double dist,
		       const Vector &delta, int pick, const BState&,
		       const Vector &pick_offset)
{
  switch(pick)
  {
  case PickSphU: case PickSphD: 
  case PickSphL: case PickSphR: 
  case PickSphI: case PickSphO:
    if (!is_aligned_)
    {
      Vector rot_start_ray_norm = rot_start_ray_;
      rot_start_ray_norm.safe_normalize();
      Vector rot_current_ray_norm = rot_start_ray_ + pick_offset;
      rot_current_ray_norm.safe_normalize();
      
      Vector rot_axis(Cross(rot_start_ray_norm, rot_current_ray_norm));

      if (rot_axis.length2()<1.e-16) 
      { 
        rot_axis=Vector(1,0,0); 
      }
      else 
      { 
        rot_axis.safe_normalize(); 
      }
      
      const Point &c = variables[CenterVar]->point();
      Transform trans;
      trans.post_translate(c.vector());
      
      const double amount = pick_offset.length() / rot_start_ray_.length();
      
      trans.post_rotate(amount, rot_axis);
      trans.post_translate(-c.vector());
      variables[PointDVar]->Move(trans.project(pick_pointdvar_));
      variables[PointRVar]->Move(trans.project(pick_pointrvar_));
      variables[PointIVar]->Move(trans.project(pick_pointivar_));
    }
    break;

  case PickResizeU:
    variables[PointRVar]->MoveDelta(delta/2.0);
    variables[PointIVar]->MoveDelta(delta/2.0);
    variables[CenterVar]->SetDelta(delta/2.0, Scheme5);
    break;

  case PickResizeR:
    variables[CenterVar]->MoveDelta(delta/2.0);
    variables[PointDVar]->MoveDelta(delta/2.0);
    variables[PointIVar]->MoveDelta(delta/2.0);
    variables[PointRVar]->SetDelta(delta, Scheme4);
    break;

  case PickResizeD:
    variables[CenterVar]->MoveDelta(delta/2.0);
    variables[PointRVar]->MoveDelta(delta/2.0);
    variables[PointIVar]->MoveDelta(delta/2.0);
    variables[PointDVar]->SetDelta(delta, Scheme5);
    break;

  case PickResizeL:
    variables[PointDVar]->MoveDelta(delta/2.0);
    variables[PointIVar]->MoveDelta(delta/2.0);
    variables[CenterVar]->SetDelta(delta/2.0, Scheme4);
    break;

  case PickResizeI:
    variables[CenterVar]->MoveDelta(delta/2.0);
    variables[PointRVar]->MoveDelta(delta/2.0);
    variables[PointDVar]->MoveDelta(delta/2.0);
    variables[PointIVar]->SetDelta(delta, Scheme6);
    break;

  case PickResizeO:
    variables[PointRVar]->MoveDelta(delta/2.0);
    variables[PointDVar]->MoveDelta(delta/2.0);
    variables[CenterVar]->SetDelta(delta/2.0, Scheme6);
    break;

  case PickSliderR:
    if (is_slideable_)
    {
      if (axis==1) dist*=-1.0;
      double sdist(variables[SDistRVar]->real()+dist/2.0);
      if (sdist<0.0) sdist=0.0;
      else if (sdist>variables[DistRVar]->real()) 
	sdist=variables[DistRVar]->real();
      variables[SDistRVar]->Set(sdist);
    }
    break;

  case PickSliderD:
    if (is_slideable_)
    {
      if (axis==1) dist*=-1.0;
      double sdist = variables[SDistDVar]->real()+dist/2.0;
      if (sdist<0.0) sdist=0.0;
      else if (sdist>variables[DistDVar]->real()) 
	sdist=variables[DistDVar]->real();
      variables[SDistDVar]->Set(sdist);
    }
    break;

  case PickSliderI:
    if (is_slideable_)
    {
      if (axis==1) dist*=-1.0;
      double sdist = variables[SDistIVar]->real()+dist/2.0;
      if (sdist<0.0) sdist=0.0;
      else if (sdist>variables[DistIVar]->real()) 
	sdist=variables[DistIVar]->real();
      variables[SDistIVar]->Set(sdist);
    }
    break;

  case PickCyls:
    variables[CenterVar]->Move(pick_centervar_);
    variables[PointDVar]->Move(pick_pointdvar_);
    variables[PointIVar]->Move(pick_pointivar_);
    variables[PointRVar]->Move(pick_pointrvar_);
    MoveDelta(pick_offset);
    break;
  }
  execute(0);
}

/***************************************************************************
 * This standard method simply moves all the widget's PointVariables by
 *      the same delta.
 * The last line of this method should call the BaseWidget execute method
 *      (which calls the redraw method).
 */
void
BoxWidget::MoveDelta( const Vector& delta )
{
  variables[CenterVar]->MoveDelta(delta);
  variables[PointRVar]->MoveDelta(delta);
  variables[PointDVar]->MoveDelta(delta);
  variables[PointIVar]->MoveDelta(delta);

  execute(1);
}


/***************************************************************************
 * This standard method returns a reference point for the widget.  This
 *      point should have some logical meaning such as the center of the
 *      widget's geometry.
 */
Point
BoxWidget::ReferencePoint() const
{
  return variables[CenterVar]->point();
}


void
BoxWidget::SetPosition( const Point& center, const Point& R, const Point& D,
			const Point& I )
{
  variables[PointRVar]->Move(R);
  variables[PointDVar]->Move(D);
  variables[PointIVar]->Move(I);
  const double sizeR((R-center).length());
  const double sizeD((D-center).length());
  const double sizeI((I-center).length());
  variables[DistRVar]->Move(sizeR);
  variables[DistDVar]->Move(sizeD);
  variables[DistIVar]->Move(sizeI);
  variables[CenterVar]->Set(center, Scheme3); // This should set Hypo...
  variables[SDistRVar]->Set(sizeR*variables[RatioRVar]->real(), Scheme1); // Slider1...
  variables[SDistDVar]->Set(sizeD*variables[RatioDVar]->real(), Scheme1); // Slider2...
  variables[SDistIVar]->Set(sizeI*variables[RatioIVar]->real(), Scheme1); // Slider3...

 
  execute(0);
}


void
BoxWidget::GetPosition( Point& center, Point& R, Point& D, Point& I )
{
  center = variables[CenterVar]->point();
  R = variables[PointRVar]->point();
  D = variables[PointDVar]->point();
  I = variables[PointIVar]->point();
}


void
BoxWidget::SetRatioR( const double ratio )
{
  ASSERT((ratio>=0.0) && (ratio<=1.0));
  variables[RatioRVar]->Set(ratio);
   
  execute(0);
}


double
BoxWidget::GetRatioR() const
{
  return (variables[RatioRVar]->real());
}


void
BoxWidget::SetRatioD( const double ratio )
{
  ASSERT((ratio>=0.0) && (ratio<=1.0));
  variables[RatioDVar]->Set(ratio);
   
  execute(0);
}


double
BoxWidget::GetRatioD() const
{
  return (variables[RatioDVar]->real());
}


void
BoxWidget::SetRatioI( const double ratio )
{
  ASSERT((ratio>=0.0) && (ratio<=1.0));
  variables[RatioIVar]->Set(ratio);
   
  execute(0);
}


double
BoxWidget::GetRatioI() const
{
  return (variables[RatioIVar]->real());
}


const Vector&
BoxWidget::GetRightAxis()
{
  Vector axis(variables[PointRVar]->point() - variables[CenterVar]->point());
  if (axis.length2() <= 1e-6)
  {
    return oldrightaxis;
  }
  else
  {
    return (oldrightaxis = axis.safe_normal());
  }
}


const Vector&
BoxWidget::GetDownAxis()
{
  Vector axis(variables[PointDVar]->point() - variables[CenterVar]->point());
  if (axis.length2() <= 1e-6)
  {
    return olddownaxis;
  }
  else
  {
    return (olddownaxis = axis.safe_normal());
  }
}


const Vector&
BoxWidget::GetInAxis()
{
  Vector axis(variables[PointIVar]->point() - variables[CenterVar]->point());
  if (axis.length2() <= 1e-6)
  {
    return oldinaxis;
  }
  else
  {
    return (oldinaxis = axis.safe_normal());
  }
}


/***************************************************************************
 * This standard method returns a string describing the functionality of
 *      a widget's material property.  The string is used in the 
 *      BaseWidget UI.
 */
std::string
BoxWidget::GetMaterialName( const Index mindex ) const
{
  ASSERT(mindex<materials.size());
   
  switch(mindex)
  {
  case 0:
    return "Point";
  case 1:
    return "Edge";
  case 2:
    return "Resize";
  case 3:
    return "Slider";
  default:
    return "UnknownMaterial";
  }
}


ClipperHandle
BoxWidget::get_clipper()
{
  Point center, right, down, in;
  GetPosition(center, right, down, in);

  // Rotate * Scale * Translate.
  Transform t;
  t.load_frame((right-center), (down-center), (in-center));
  t.pre_translate(Vector(center.x(), center.y(), center.z()));
  t.invert();

  return new BoxClipper(t);
}


} // End namespace SCIRun
