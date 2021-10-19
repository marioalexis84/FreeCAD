/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *   for detail see the LICENCE text file.                                 *
 *   Jürgen Riegel 2002                                                    *
 *                                                                         *
 ***************************************************************************/

#include "PreCompiled.h"
#ifndef _PreComp_
# include <QAction>
# include <QFileDialog>
# include <QImage>
# include <QImageReader>
# include <QMessageBox>
# include <QTextStream>
#endif

#include <time.h>
#if defined(FC_OS_WIN32)
#include <sys/timeb.h>
#endif

#include <Base/Exception.h>
#include <Base/Tools.h>
#include <Base/Interpreter.h>
#include <App/Document.h>
#include <Gui/Application.h>
#include <Gui/MainWindow.h>
#include <Gui/Command.h>
#include <Gui/BitmapFactory.h>
#include "ImageOrientationDialog.h"

#if HAVE_OPENCV2
#  include "opencv2/opencv.hpp"
#endif

#include "ImageView.h"
#include "Mod/Image/App/ImagePipeline.h"
#include <Base/Console.h>
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

using namespace ImageGui;

DEF_STD_CMD(CmdImageOpen)

CmdImageOpen::CmdImageOpen()
  : Command("Image_Open")
{
    sAppModule      = "Image";
    sGroup          = QT_TR_NOOP("Image");
    sMenuText       = QT_TR_NOOP("Open...");
    sToolTipText    = QT_TR_NOOP("Open image view");
    sWhatsThis      = "Image_Open";
    sStatusTip      = sToolTipText;
    sPixmap         = "Image_Open";
}

void CmdImageOpen::activated(int iMsg)
{
    Q_UNUSED(iMsg);

    // add all supported QImage formats
    QString formats;
    QTextStream str(&formats);
    str << QObject::tr("Images") << " (";
    QList<QByteArray> qtformats = QImageReader::supportedImageFormats();
    for (QList<QByteArray>::Iterator it = qtformats.begin(); it != qtformats.end(); ++it) {
        str << "*." << it->toLower() << " ";
    }
    str << ");;" << QObject::tr("All files") << " (*.*)";
    // Reading an image
    QString s = QFileDialog::getOpenFileName(Gui::getMainWindow(), QObject::tr("Choose an image file to open"),
                                             QString(), formats);
    if (!s.isEmpty()) {
        try {
            s = Base::Tools::escapeEncodeFilename(s);
            // load the file with the module
            Command::doCommand(Command::Gui, "import Image, ImageGui");
            Command::doCommand(Command::Gui, "ImageGui.open(\"%s\",\"utf-8\")", (const char*)s.toUtf8());
        }
        catch (const Base::PyException& e){
            // Usually thrown if the file is invalid somehow
            e.ReportException();
        }
    }
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
DEF_STD_CMD_A(CmdCreateImagePlane)

CmdCreateImagePlane::CmdCreateImagePlane()
    :Command("Image_CreateImagePlane")
{
    sAppModule      = "Image";
    sGroup          = QT_TR_NOOP("Image");
    sMenuText       = QT_TR_NOOP("Create image plane...");
    sToolTipText    = QT_TR_NOOP("Create a planar image in the 3D space");
    sWhatsThis      = "Image_CreateImagePlane";
    sStatusTip      = sToolTipText;
    sPixmap         = "Image_CreateImagePlane";
}

void CmdCreateImagePlane::activated(int iMsg)
{
    Q_UNUSED(iMsg);

    QString formats;
    QTextStream str(&formats);
    str << QObject::tr("Images") << " (";
    QList<QByteArray> qtformats = QImageReader::supportedImageFormats();
    for (QList<QByteArray>::Iterator it = qtformats.begin(); it != qtformats.end(); ++it) {
        str << "*." << it->toLower() << " ";
    }
    str << ");;" << QObject::tr("All files") << " (*.*)";
    // Reading an image
    QString s = QFileDialog::getOpenFileName(Gui::getMainWindow(), QObject::tr("Choose an image file to open"),
                                             QString(), formats);
    if (!s.isEmpty()) {

        QImage impQ(s);
        if (impQ.isNull()) {
            QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Error opening image"),
                QObject::tr("Could not load the chosen image"));
            return;
        }

        // ask user for orientation
        ImageOrientationDialog Dlg;

        if (Dlg.exec() != QDialog::Accepted)
            return; // canceled
        Base::Vector3d p = Dlg.Pos.getPosition();
        Base::Rotation r = Dlg.Pos.getRotation();

        std::string FeatName = getUniqueObjectName("ImagePlane");
        double xPixelsPerM = impQ.dotsPerMeterX();
        double width = impQ.width();
        width = width * 1000 / xPixelsPerM;
        double yPixelsPerM = impQ.dotsPerMeterY();
        double height = impQ.height();
        height = height * 1000 / yPixelsPerM;

        QString pyfile = Base::Tools::escapeEncodeFilename(s);

        openCommand(QT_TRANSLATE_NOOP("Command", "Create ImagePlane"));
        doCommand(Doc,"App.activeDocument().addObject('Image::ImagePlane','%s\')",FeatName.c_str());
        doCommand(Doc,"App.activeDocument().%s.ImageFile = '%s'",FeatName.c_str(),(const char*)pyfile.toUtf8());
        doCommand(Doc,"App.activeDocument().%s.XSize = %f",FeatName.c_str(),width);
        doCommand(Doc,"App.activeDocument().%s.YSize = %f",FeatName.c_str(),height);
        doCommand(Doc,"App.activeDocument().%s.Placement = App.Placement(App.Vector(%f,%f,%f),App.Rotation(%f,%f,%f,%f))"
                     ,FeatName.c_str(),p.x,p.y,p.z,r[0],r[1],r[2],r[3]);
        doCommand(Doc,"Gui.SendMsgToActiveView('ViewFit')");
        commitCommand();
    }
}

bool CmdCreateImagePlane::isActive()
{
    return App::GetApplication().getActiveDocument();
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
DEF_STD_CMD(CmdImageScaling)

CmdImageScaling::CmdImageScaling()
  : Command("Image_Scaling")
{
    sAppModule      = "Image";
    sGroup          = QT_TR_NOOP("Image");
    sMenuText       = QT_TR_NOOP("Scale...");
    sToolTipText    = QT_TR_NOOP("Image Scaling");
    sWhatsThis      = "Image_Scaling";
    sStatusTip      = sToolTipText;
    sPixmap         = "Image_Scaling";
}

void CmdImageScaling::activated(int iMsg)
{
    Q_UNUSED(iMsg);
    // To Be Defined

}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#if HAVE_OPENCV2
DEF_STD_CMD(CmdImageCapturerTest);

CmdImageCapturerTest::CmdImageCapturerTest()
  : Command("Image_CapturerTest")
{
    sAppModule      = "Image";
    sGroup          = ("Image");
    sMenuText       = ("CapturerTest");
    sToolTipText    = ("test camara capturing");
    sWhatsThis      = "Image_CapturerTest";
    sStatusTip      = sToolTipText;
    sPixmap         = "camera-photo";
}

void CmdImageCapturerTest::activated(int iMsg)
{
    Q_UNUSED(iMsg)
	using namespace cv;

    VideoCapture cap(0); // open the default camera
    if(!cap.isOpened())  // check if we succeeded
        return;

    Mat edges;
    namedWindow("edges",1);
    for(;;)
    {
        Mat frame;
        cap >> frame; // get a new frame from camera
        cvtColor(frame, edges, COLOR_BGR2RGB);
//        GaussianBlur(edges, edges, Size(7,7), 1.5, 1.5);
//        Canny(edges, edges, 0, 30, 3);
        imshow("edges", edges);
        if(waitKey(30) >= 0) break;
    }
    // the camera will be deinitialized automatically in VideoCapture destructor

}
#endif

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
DEF_STD_CMD_A(CmdCreateImageObject)

CmdCreateImageObject::CmdCreateImageObject()
    :Command("Image_CreateImageObject")
{
    sAppModule      = "Image";
    sGroup          = QT_TR_NOOP("Image");
    sMenuText       = QT_TR_NOOP("Image object");
    sToolTipText    = QT_TR_NOOP("Create image object");
    sWhatsThis      = "Image_CreateImageObject";
    sStatusTip      = sToolTipText;
    sPixmap         = "";
}

void CmdCreateImageObject::activated(int iMsg)
{
    Q_UNUSED(iMsg);

    QString formats;
    QTextStream str(&formats);
    str << QObject::tr("Images") << " (";
    QList<QByteArray> qtformats = QImageReader::supportedImageFormats();
    for (QList<QByteArray>::Iterator it = qtformats.begin(); it != qtformats.end(); ++it) {
        str << "*." << it->toLower() << " ";
    }
    str << ");;" << QObject::tr("All files") << " (*.*)";
    // Reading an image
    QString s = QFileDialog::getOpenFileName(Gui::getMainWindow(), QObject::tr("Choose an image file to open"),
                                             QString(), formats);
    if (!s.isEmpty()) {

//        QImage impQ(s);
//        if (impQ.isNull()) {
//            QMessageBox::warning(Gui::getMainWindow(), QObject::tr("Error opening image"),
//                QObject::tr("Could not load the chosen image"));
//            return;
//        }
        std::string FeatName = getUniqueObjectName("ImageObject");
        QString pyfile = Base::Tools::escapeEncodeFilename(s);

        openCommand(QT_TRANSLATE_NOOP("Command", "Create ImageObject"));
        doCommand(Doc,"App.activeDocument().addObject('Image::ImageObject','%s\')",FeatName.c_str());
        doCommand(Doc,"App.activeDocument().%s.read('%s')",FeatName.c_str(), pyfile.toUtf8().data());
        commitCommand();
    }
}

bool CmdCreateImageObject::isActive()
{
    return App::GetApplication().getActiveDocument();
}

//****************************************************************************
DEF_STD_CMD_A(CmdCreateImagePipeline)

CmdCreateImagePipeline::CmdCreateImagePipeline()
    :Command("Image_CreateImagePipeline")
{
    sAppModule      = "Image";
    sGroup          = QT_TR_NOOP("Image");
    sMenuText       = QT_TR_NOOP("Pipeline");
    sToolTipText    = QT_TR_NOOP("Create image pipeline");
    sWhatsThis      = "Image_CreateImagePipeline";
    sStatusTip      = sToolTipText;
    sPixmap         = "";
}

void CmdCreateImagePipeline::activated(int iMsg)
{
    Q_UNUSED(iMsg);

    std::string FeatName = getUniqueObjectName("ImagePipeline");

    openCommand(QT_TRANSLATE_NOOP("Command", "Create ImagePipeline"));
    doCommand(Doc,"App.activeDocument().addObject('Image::ImagePipeline','%s\')",FeatName.c_str());
    commitCommand();
}

bool CmdCreateImagePipeline::isActive()
{
    return App::GetApplication().getActiveDocument();
}

void addToPipeline(Gui::Command& cmd, const char* objType)
{
    Base::Type container = Image::ImagePipeline::getClassTypeId();
    std::vector<App::DocumentObject*> select = Gui::Selection().getObjectsOfType(container, nullptr, 0);
    if (select.size() != 1) {
        QMessageBox::warning(Gui::getMainWindow(), QObject::tr("ImagePipeline error"),
            QString::fromUtf8("Single image pipeline must be selected"));
        return;
    }
    const char* pipeName = select.front()->getNameInDocument();
    const char* featName = cmd.getUniqueObjectName(objType).c_str();
    Base::Console().Message("nombre:%s\n", featName);
    cmd.openCommand(QT_TRANSLATE_NOOP("Command", "Create "));
    cmd.doCommand(cmd.Doc, "App.activeDocument().addObject('Image::%s', '%s')", objType, featName);
    cmd.doCommand(cmd.Doc, "App.activeDocument().getObject('%s').addObject(App.activeDocument().getObject('%s'))", pipeName, featName);
    cmd.commitCommand();
}

//****************************************************************************
DEF_STD_CMD_A(CmdCreateImageCanny)

CmdCreateImageCanny::CmdCreateImageCanny()
    :Command("Image_CreateImageCanny")
{
    sAppModule      = "Image";
    sGroup          = QT_TR_NOOP("Image");
    sMenuText       = QT_TR_NOOP("Canny");
    sToolTipText    = QT_TR_NOOP("Apply Canny filter on image");
    sWhatsThis      = "Image_CreateImageCanny";
    sStatusTip      = sToolTipText;
    sPixmap         = "";
}

void CmdCreateImageCanny::activated(int iMsg)
{
    Q_UNUSED(iMsg);
    addToPipeline(*this, "ImageCanny");
}

bool CmdCreateImageCanny::isActive()
{
    return App::GetApplication().getActiveDocument();
}

//****************************************************************************
DEF_STD_CMD_A(CmdCreateImageColor)

CmdCreateImageColor::CmdCreateImageColor()
    :Command("Image_CreateImageColor")
{
    sAppModule      = "Image";
    sGroup          = QT_TR_NOOP("Image");
    sMenuText       = QT_TR_NOOP("ColorSpace");
    sToolTipText    = QT_TR_NOOP("Set image color space");
    sWhatsThis      = "Image_CreateImageColor";
    sStatusTip      = sToolTipText;
    sPixmap         = "";
}

void CmdCreateImageColor::activated(int iMsg)
{
    Q_UNUSED(iMsg);
    addToPipeline(*this, "ImageColor");
}

bool CmdCreateImageColor::isActive()
{
    return App::GetApplication().getActiveDocument();
}

//****************************************************************************
DEF_STD_CMD_A(CmdCreateImageContour)

CmdCreateImageContour::CmdCreateImageContour()
    :Command("Image_CreateImageContour")
{
    sAppModule      = "Image";
    sGroup          = QT_TR_NOOP("Image");
    sMenuText       = QT_TR_NOOP("Contour");
    sToolTipText    = QT_TR_NOOP("Find contours on image");
    sWhatsThis      = "Image_CreateImageContour";
    sStatusTip      = sToolTipText;
    sPixmap         = "";
}

void CmdCreateImageContour::activated(int iMsg)
{
    Q_UNUSED(iMsg);
    addToPipeline(*this, "ImageContour");
}

bool CmdCreateImageContour::isActive()
{
    return App::GetApplication().getActiveDocument();
}

//****************************************************************************
DEF_STD_CMD_A(CmdCreateImageThreshold)

CmdCreateImageThreshold::CmdCreateImageThreshold()
    :Command("Image_CreateImageThreshold")
{
    sAppModule      = "Image";
    sGroup          = QT_TR_NOOP("Image");
    sMenuText       = QT_TR_NOOP("Threshold");
    sToolTipText    = QT_TR_NOOP("Set threshold on binary image");
    sWhatsThis      = "Image_CreateImageThreshold";
    sStatusTip      = sToolTipText;
    sPixmap         = "";
}

void CmdCreateImageThreshold::activated(int iMsg)
{
    Q_UNUSED(iMsg);
    addToPipeline(*this, "ImageThreshold");
}

bool CmdCreateImageThreshold::isActive()
{
    return App::GetApplication().getActiveDocument();
}

//****************************************************************************
void CreateImageCommands(void)
{
    Gui::CommandManager &rcCmdMgr = Gui::Application::Instance->commandManager();

    rcCmdMgr.addCommand(new CmdImageOpen());
    rcCmdMgr.addCommand(new CmdCreateImagePlane());
#if HAVE_OPENCV2
	rcCmdMgr.addCommand(new CmdImageCapturerTest());
	rcCmdMgr.addCommand(new CmdCreateImageObject());
	rcCmdMgr.addCommand(new CmdCreateImagePipeline());
	rcCmdMgr.addCommand(new CmdCreateImageCanny());
	rcCmdMgr.addCommand(new CmdCreateImageColor());
	rcCmdMgr.addCommand(new CmdCreateImageContour());
	rcCmdMgr.addCommand(new CmdCreateImageThreshold());
#endif
}

