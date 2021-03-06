#include "ToolPageLayer.h"

#include "gui/GladeGui.h"

#include <config.h>
#include <i18n.h>

ToolPageLayer::ToolPageLayer(GladeGui* gui, ActionHandler* handler, string id, ActionType type) :
		AbstractToolItem(id, handler, type, NULL)
{
	XOJ_INIT_TYPE(ToolPageLayer);

	this->layerComboBox = gtk_combo_box_text_new();
	this->layerCount = -5;
	this->inCbUpdate = false;
	this->gui = gui;

	g_signal_connect(this->layerComboBox, "changed", G_CALLBACK(&cbSelectCallback), this);
}

ToolPageLayer::~ToolPageLayer()
{
	XOJ_RELEASE_TYPE(ToolPageLayer);
}

void ToolPageLayer::cbSelectCallback(GtkComboBox* widget, ToolPageLayer* tpl)
{
	XOJ_CHECK_TYPE_OBJ(tpl, ToolPageLayer);

	if (tpl->inCbUpdate)
	{
		return;
	}

	tpl->handler->actionPerformed(ACTION_FOOTER_LAYER, GROUP_NOGROUP, NULL, NULL, NULL, true);
}

int ToolPageLayer::getSelectedLayer()
{
	XOJ_CHECK_TYPE(ToolPageLayer);

	GtkComboBox* cb = GTK_COMBO_BOX(layerComboBox);
	int count = gtk_tree_model_iter_n_children(gtk_combo_box_get_model(cb), NULL);
	int selected = count - gtk_combo_box_get_active(cb) - 1;
	return selected;
}

void ToolPageLayer::setSelectedLayer(int selected)
{
	XOJ_CHECK_TYPE(ToolPageLayer);

	GtkComboBox* cb = GTK_COMBO_BOX(layerComboBox);
	int count = gtk_tree_model_iter_n_children(gtk_combo_box_get_model(cb), NULL);

	gtk_combo_box_set_active(cb, count - selected - 1);
}

void ToolPageLayer::setLayerCount(int layer, int selected)
{
	XOJ_CHECK_TYPE(ToolPageLayer);

	this->inCbUpdate = true;

	int count = gtk_tree_model_iter_n_children(gtk_combo_box_get_model(GTK_COMBO_BOX(this->layerComboBox)), NULL);

	for (int i = count - 1; i >= 0; i--)
	{
		gtk_combo_box_text_remove(GTK_COMBO_BOX_TEXT(this->layerComboBox), i);
	}

	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(this->layerComboBox), _C("Background"));
	for (int i = 1; i <= layer; i++)
	{
		string text = FS(_F("Layer {1}") % i);
		gtk_combo_box_text_prepend_text(GTK_COMBO_BOX_TEXT(this->layerComboBox), text.c_str());
	}

	setSelectedLayer(selected);

	this->layerCount = layer;
	this->inCbUpdate = false;
}

string ToolPageLayer::getToolDisplayName()
{
	XOJ_CHECK_TYPE(ToolPageLayer);

	return _("Layer selection");
}

GtkWidget* ToolPageLayer::getNewToolIcon()
{
	XOJ_CHECK_TYPE(ToolPageLayer);

	return gui->loadIcon("layers.svg");
}

GtkToolItem* ToolPageLayer::newItem()
{
	XOJ_CHECK_TYPE(ToolPageLayer);

	GtkToolItem* it = gtk_tool_item_new();

	GtkWidget* hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 1);
	gtk_box_pack_start(GTK_BOX(hbox), gtk_label_new(_C("Layer")), false, false, 7);

	gtk_box_pack_start(GTK_BOX(hbox), this->layerComboBox, false, false, 0);

	gtk_container_add(GTK_CONTAINER(it), hbox);

	return it;
}
