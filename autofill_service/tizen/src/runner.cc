#include "runner.h"

#include "generated_plugin_registrant.h"

#include <autofill_service.h>

#include <string>
#include <vector>
#include <memory>

class AutofillItem
{
public:
	AutofillItem() {}
	AutofillItem(std::string id, std::string label, std::string value, autofill_hint_e hint, bool sensitive)
	{
		id_ = id, label_ = label, value_ = value, autofill_hint_ = hint, sensitive_data_ = sensitive;
	}
	std::string Id() { return id_; }
	void SetId(std::string id) { id_ = id; }
	std::string Label() { return label_; }
	void SetLabel(std::string label) { label_ = label; }
	std::string Value() { return value_; }
	void SetValue(std::string value) { value_ = value; }
	autofill_hint_e AutofillHint() { return autofill_hint_; }
	void SetAutofillHint(autofill_hint_e autofill_hint) { autofill_hint_ = autofill_hint; }
	bool SensitiveData() { return sensitive_data_; }
	void SetSensitiveData(bool sensitive_data) { sensitive_data_ = sensitive_data; }

private:
	std::string id_;
	std::string label_;
	std::string value_;
	autofill_hint_e autofill_hint_;
	bool sensitive_data_;
};

class TizenAutofillService
{
public:
	TizenAutofillService()
	{
		initializeAutofillService();
		AddAutofillItem(AutofillItem("id A", "label A", "value A", AUTOFILL_HINT_ID, true));
		AddAutofillItem(AutofillItem("id B", "label B", "value B", AUTOFILL_HINT_ID, true));
		AddAutofillItem(AutofillItem("id C", "label C", "value C", AUTOFILL_HINT_ID, true));
		AddAutofillItem(AutofillItem("id D", "label D", "value D", AUTOFILL_HINT_ID, true));
	}

	~TizenAutofillService()
	{
		autofill_service_deinitialize();
	}

	void AddAutofillItem(AutofillItem item)
	{
		items_.push_back(item);
	}

	std::vector<AutofillItem> getAutofillItems()
	{
		return items_;
	}

private:
	static void send_auth_info(int context_id, const char *app_id, const char *view_id)
	{
		/* Create authentication information */
		autofill_auth_info_h auth_info;
		autofill_auth_info_create(&auth_info);
		autofill_auth_info_set_app_id(auth_info, app_id);
		autofill_auth_info_set_view_id(auth_info, view_id);

		autofill_auth_info_set_autofill_data_present(auth_info, true);
		autofill_auth_info_set_authentication_needed(auth_info, true);
		autofill_auth_info_set_service_name(auth_info, "My autofill service");
		autofill_auth_info_set_service_message(auth_info, "My autofill service message");
		autofill_auth_info_set_service_logo_image_path(auth_info, "");

		/* Send authentication information */
		autofill_service_send_auth_info(context_id, auth_info);

		autofill_auth_info_destroy(auth_info);
	}

	void initializeAutofillService()
	{
		autofill_service_initialize();

		autofill_service_set_auth_info_requested_cb([](int context_id, autofill_view_info_h vi_h, void *user_data)
													{
			char *view_id = NULL;
			char *app_id = NULL;

			autofill_view_info_get_app_id(vi_h, &app_id);
			autofill_view_info_get_view_id(vi_h, &view_id);

			TizenAutofillService::send_auth_info(context_id, app_id, view_id);

			if (app_id)
				free(app_id);

			if (view_id)
				free(view_id); },
													NULL);
		autofill_service_set_fill_requested_cb([](int context_id, autofill_view_info_h vi_h, void *user_data)
											   {
			char *app_id = NULL;
			char *view_id = NULL;

			// /* Get application ID and view ID sent the request of authentication information */
			autofill_view_info_get_app_id(vi_h, &app_id);
			autofill_view_info_get_view_id(vi_h, &view_id);

			// /* Retrieve the stored user information */

			// /* Create fill response */
			autofill_fill_response_h fill_response_h;
			autofill_fill_response_create(&fill_response_h);
			autofill_fill_response_set_app_id(fill_response_h, app_id);
			autofill_fill_response_set_view_id(fill_response_h, view_id);

			// autofill_fill_response_group_h res_group_h[2];
			// autofill_fill_response_item_h res_it_h[2];
			if (app_id)
				free(app_id);

			if (view_id)
				free(view_id);

			std::vector<autofill_hint_e> hints;
      		std::vector<AutofillItem> items = static_cast<TizenAutofillService*>(user_data)->getAutofillItems();

			autofill_view_info_foreach_item(
				vi_h, [](autofill_item_h item, void *user_data)
			{
				autofill_hint_e hint;
				autofill_item_get_autofill_hint(item, &hint);
				std::vector<autofill_hint_e>* hints = static_cast<std::vector<autofill_hint_e>*>(user_data);
				hints->push_back(hint);
				return true; },
				static_cast<void *>(&hints));

			for (auto hint : hints)
			{
				autofill_fill_response_group_h response_group;
				autofill_fill_response_group_create(&response_group);
				for (auto item : items)
				{
					if (item.AutofillHint() == hint)
					{
						// TODO : change more efficiently, maybe using having vectors each autofill hint
						autofill_fill_response_item_h response_item;
						autofill_fill_response_item_create(&response_item);
						autofill_fill_response_item_set_id(response_item, item.Id().c_str());
						autofill_fill_response_item_set_presentation_text(response_item, item.Label().c_str());
						autofill_fill_response_item_set_value(response_item, item.Value().c_str());
						autofill_fill_response_item_set_autofill_hint(response_item, item.AutofillHint());

						autofill_fill_response_group_add_item(response_group, response_item);
						autofill_fill_response_item_destroy(response_item);
					}
				}
				autofill_fill_response_add_group(fill_response_h, response_group);
			}

			autofill_service_send_fill_response(context_id, fill_response_h);

			autofill_fill_response_destroy(fill_response_h); },
											   this);
		autofill_service_set_committed_cb([](int context_id, autofill_save_view_info_h vi_h, void *user_data)
										  {char *view_id = NULL;
		autofill_save_view_info_get_view_id(vi_h, &view_id);
		autofill_save_view_info_foreach_item(vi_h, [](autofill_save_item_h it_h, void *user_data)
		{
			char *id = NULL;
			char *label = NULL;
			char *value = NULL;
			autofill_hint_e autofill_hint;
			bool sensitive_data;

			autofill_save_item_get_id(it_h, &id);
			autofill_save_item_get_label(it_h, &label);
			autofill_save_item_get_value(it_h, &value);
			autofill_save_item_get_autofill_hint(it_h, &autofill_hint);
			autofill_save_item_get_sensitive_data(it_h, &sensitive_data);

			AutofillItem item;
			item.SetId(std::string(id));
			item.SetLabel(std::string(label));
			item.SetValue(std::string(value));
			item.SetAutofillHint(autofill_hint);
			item.SetSensitiveData(sensitive_data);

			TizenAutofillService *service = static_cast<TizenAutofillService *>(user_data);

			service->AddAutofillItem(item);

			free(id);
			free(label);
			free(value);

			return true;
		}, user_data);

		if (view_id)
			free(view_id); },
										  this);
	}

	std::vector<AutofillItem> items_;
};

class App : public FlutterServiceApp
{
public:
	~App()
	{
	}

	bool OnCreate()
	{
		if (FlutterServiceApp::OnCreate())
		{
			RegisterPlugins(this);
		}

		return IsRunning();
	}

	TizenAutofillService autofill_service;
};

int main(int argc, char *argv[])
{
	App app;
	return app.Run(argc, argv);
}
