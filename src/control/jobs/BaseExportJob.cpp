#include "BaseExportJob.h"

#include "control/Control.h"

#include <i18n.h>

#include <boost/algorithm/string/predicate.hpp>

BaseExportJob::BaseExportJob(Control* control, string name)
 : BlockingJob(control, name),
   dialog(NULL)
{
	XOJ_INIT_TYPE(BaseExportJob);
}

BaseExportJob::~BaseExportJob()
{
	XOJ_CHECK_TYPE(BaseExportJob);

	XOJ_RELEASE_TYPE(BaseExportJob);
}

void BaseExportJob::initDialog()
{
	dialog = gtk_file_chooser_dialog_new(_C("Export PDF"), control->getGtkWindow(), GTK_FILE_CHOOSER_ACTION_SAVE,
													_C("_Cancel"), GTK_RESPONSE_CANCEL,
													_C("_Save"), GTK_RESPONSE_OK, NULL);

	gtk_file_chooser_set_local_only(GTK_FILE_CHOOSER(dialog), true);
}

void BaseExportJob::addFileFilterToDialog(string name, string pattern)
{
	GtkFileFilter* filter = gtk_file_filter_new();
	gtk_file_filter_set_name(filter, name.c_str());
	gtk_file_filter_add_pattern(filter, pattern.c_str());
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);
}

void BaseExportJob::prepareSavePath(path& path)
{
	XOJ_CHECK_TYPE(BaseExportJob);
	// Nothing to do here, but will be overwritten
}

bool BaseExportJob::showFilechooser()
{
	XOJ_CHECK_TYPE(BaseExportJob);

	initDialog();
	addFilterToDialog();

	path savePath;

	Settings* settings = control->getSettings();
	Document* doc = control->getDocument();
	doc->lock();
	if (!doc->getFilename().empty())
	{
		savePath = doc->getFilename();
	}
	else if (!doc->getPdfFilename().empty())
	{
		savePath = doc->getPdfFilename().filename().replace_extension("");
	}
	else
	{
		time_t curtime = time(NULL);
		char stime[128];
		strftime(stime, sizeof(stime), settings->getDefaultSaveName().c_str(), localtime(&curtime));
		savePath /= stime;
	}
	doc->unlock();

	prepareSavePath(savePath);

	GFile* folder = g_file_new_for_path(savePath.parent_path().c_str());
	gtk_file_chooser_set_current_folder_uri(GTK_FILE_CHOOSER(dialog), g_file_get_uri(folder));
	g_object_unref(folder);

	gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(dialog), savePath.filename().c_str());
	gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER(dialog), true);

	gtk_window_set_transient_for(GTK_WINDOW(dialog), GTK_WINDOW(this->control->getWindow()->getWindow()));

	while (true)
	{
		if (gtk_dialog_run(GTK_DIALOG(dialog)) != GTK_RESPONSE_OK)
		{
			gtk_widget_destroy(dialog);
			return false;
		}

		string uri(gtk_file_chooser_get_uri(GTK_FILE_CHOOSER(dialog)));
		this->filename = path(gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog)));

		if (isUriValid(uri))
		{
			break;
		}
	}

	settings->setLastSavePath(this->filename.parent_path());

	gtk_widget_destroy(dialog);

	return true;
}

bool BaseExportJob::isUriValid(string& uri)
{
	XOJ_CHECK_TYPE(BaseExportJob);

	if (!ba::starts_with(uri, "file://"))
	{
		string msg = (_F("Only local files are supported\nPath: {1}") % uri).str();
		Util::showErrorToUser(control->getGtkWindow(), msg);
		return false;
	}

	return true;
}

void BaseExportJob::afterRun()
{
	XOJ_CHECK_TYPE(BaseExportJob);

	if (!this->errorMsg.empty())
	{
		Util::showErrorToUser(control->getGtkWindow(), this->errorMsg);
	}
}

