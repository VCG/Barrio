#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QTreeWidgetItem>
#include <QMainWindow>
#include <QStandardItemModel>
#include <QListWidgetItem>
#include "inputform.h"
#include "mainwidget.h"
#include "treemodel.h"

namespace Ui {
  class MainWindow;
}

class GLWidget;
class MousePad;
namespace Clustering {
  class GlycogenCluster;
}

class MainWindow : public QMainWindow
{
  Q_OBJECT // for signals and slots

public:
  explicit MainWindow(QWidget* parent = 0, InputForm* input_form = NULL);
  ~MainWindow();

  void setupSignalsNSlots();

  void onAddWebWidget();

public slots:

  void on_mitoRadioButton_toggled(bool);
  void on_boutonsSpinesRadioButton_toggled(bool);

  void on_glycogenMappingTreeWidget_itemChanged(QTreeWidgetItem* item, int column);

  //----- selected objects
  void on_object_clicked(QList<QStandardItem*>);
  void clearTable();
  void RemoveRowAt(QModelIndex);
  void getIDAt(QModelIndex);

  // Path Management
  void getPath(QString);
  void getSelectedPathIndex(QModelIndex);
  void deleteSelectedPath(QModelIndex);

  void on_filterByProximityListWidget_itemChanged(QListWidgetItem*);

  void on_listWidget_itemChanged(QListWidgetItem*);

  void on_structure_selection_changed(int state);

  void on_high_detail_vis_clicked();
  void on_medium_detail_vis_clicked();
  void on_low_detail_vis_clicked();

  void on_synapse_distance_slider_changed(int value);
  void on_opacity_slider_changed(int value);



signals:
  void getDeletedData(int);
  void getIDAtSelectedRow(int);

  // path
  void signalPathLabel(QString);
  void signalDeletedPathLabel(QString);

  void glycogenMappedSelectedState(QString, bool);
  void signalProximityTypeState(QString, bool);
  void signalMappingTreeWidget(QTreeWidget*);
  void update_glycogen_clustering_timing(QString);
  void update_glycogen_cluter_mapping_timing(QString);
  void update_glycogen_granules_mapping_timing(QString);


private:
  Ui::MainWindow* mainwindow_ui;
  std::map<int, Clustering::GlycogenCluster*>* m_clusters;
  QTreeWidgetItem* m_currentSelectedCluster;
  QStandardItemModel* tableView;

  MainWidget* m_mainWidget;
  TreeModel* m_treemodel;
  DataContainer* m_data_container;

  void initializeSynapseThresholdSlider();
  void initializeOpacitySlider();
  void initializeVisualizationPresets()l

};

#endif // MAINWINDOW_H
