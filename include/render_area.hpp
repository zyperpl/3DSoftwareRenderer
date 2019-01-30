#pragma once

#include <QBrush>
#include <QPen>
#include <QPixmap>
#include <QWidget>
#include <QImage>
#include <QPainter>
#include <QVector2D>
#include <QVector>

#include <memory>

#include "camera.hpp"
#include "model.hpp"

class RenderArea : public QWidget
{
  Q_OBJECT
  
  public:
    RenderArea(QWidget *parent);
    void paintEvent(QPaintEvent *event);
    void keyPressEvent(QKeyEvent *event);

    std::unique_ptr<Model>  model;
    std::unique_ptr<Camera> camera;
};
