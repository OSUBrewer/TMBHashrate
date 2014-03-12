#include <pebble.h>
#include <xprintf.h>

Window *window;
TextLayer *dogecoin_layer;
TextLayer *legend_layer_TMB;
TextLayer *legend_layer_scrypt;
TextLayer *legend_layer_scryptn;
TextLayer *time_layer;
TextLayer *date_layer;
TextLayer *wow_layer;
TextLayer *amaze_layer;
BitmapLayer *image_layer;
GBitmap *image;
int tmbHash;
int tmbScrypt;
int tmbScryptn;


void drawSPD(int tmbHash){
	static char tickerText[] = "Xxxxxxxxxx";
	static char TMB_Text[] = "TMB";
	static char scryptText[] = "loading";
	static char scryptnText[] = "loading";
	if (tmbHash == -2) {
		xsprintf(tickerText, "Loading");
		text_layer_set_font(dogecoin_layer,
							fonts_load_custom_font(resource_get_handle(RESOURCE_ID_proto_26)));
		xsprintf(TMB_Text, "TMB");
		text_layer_set_font(legend_layer_TMB,
							fonts_load_custom_font(resource_get_handle(RESOURCE_ID_proto_18)));
		
	} else if (tmbHash < 0) {
		xsprintf(tickerText, "err");
		text_layer_set_font(dogecoin_layer,
							fonts_load_custom_font(resource_get_handle(RESOURCE_ID_proto_26)));
		xsprintf(TMB_Text, "TMB");
		text_layer_set_font(legend_layer_TMB,
							fonts_load_custom_font(resource_get_handle(RESOURCE_ID_proto_18)));
	} else {
		xsprintf(tickerText, "%d kH/s", tmbHash);
		text_layer_set_font(dogecoin_layer,
							fonts_load_custom_font(resource_get_handle(RESOURCE_ID_proto_26)));
		xsprintf(TMB_Text, "TMB");
		text_layer_set_font(legend_layer_TMB,
							fonts_load_custom_font(resource_get_handle(RESOURCE_ID_proto_18)));
		
	}
	// TODO store last values, don't update if they didn't change
	// (to save battery life)
	text_layer_set_text(dogecoin_layer, tickerText);
	
	//layer_mark_dirty(bitmap_layer_get_layer(image_layer));
}

void drawTime(struct tm *tick_time, TimeUnits units_changed){
	static char time_text[20];
	strftime(time_text, sizeof(time_text), "%R %p", tick_time);
	// strip leading 0
	if (time_text[0] == '0'){
		for (int i=0; i < (int) sizeof(time_text)-1; i++){
			time_text[i] = time_text[i+1];
			if (time_text[i] == 0){
				break;
			}
		}
	}
	text_layer_set_text(time_layer, time_text);

	static char date_text[40];
	strftime(date_text, sizeof(date_text), "%A %b %d", tick_time);
	text_layer_set_text(date_layer, date_text);
}

void inbox_recieved_callback(DictionaryIterator *iterator, void *context){
	bool updated = 0;
	Tuple* tuple = dict_find(iterator, 0);
	if (tuple != 0){
		if (tuple->type == TUPLE_INT && tuple->length == 4){
			tmbHash = tuple->value->int32;
		} else {
			tmbHash = -1;
		}
		updated = 1;
	}
	if (updated){
		drawSPD(tmbHash);
	}
}

void handle_init(void) {
	window = window_create();
	window_stack_push(window, true /* Animated */);
	
	Layer *window_layer = window_get_root_layer(window);
	
	GRect bounds = layer_get_frame(window_layer);

	
	legend_layer_TMB = text_layer_create(GRect(0, 0, 144, 30)); /*GRect(40, 3, 144-40-10, 20)*/
	text_layer_set_font(legend_layer_TMB,
					   fonts_load_custom_font(resource_get_handle(RESOURCE_ID_proto_18)));
	text_layer_set_text_color(legend_layer_TMB, GColorBlack);
	text_layer_set_text_alignment(legend_layer_TMB, GTextAlignmentCenter);
	layer_add_child(window_layer, text_layer_get_layer(legend_layer_TMB));
	
	legend_layer_scrypt = text_layer_create(GRect(0, 60, 144, 80));  /*GRect(40, 24, 144-40-10, 20) */
	text_layer_set_font(legend_layer_scrypt,
					   fonts_load_custom_font(resource_get_handle(RESOURCE_ID_proto_18)));
	text_layer_set_text_color(legend_layer_scrypt, GColorBlack);
	text_layer_set_text_alignment(legend_layer_scrypt, GTextAlignmentCenter);
	layer_add_child(window_layer, text_layer_get_layer(legend_layer_scrypt));
	
	dogecoin_layer = text_layer_create(GRect(0, 30, 144, 30)); /*GRect(7, 3, 55-7, 168-3)*/
	text_layer_set_font(dogecoin_layer,
					   fonts_load_custom_font(resource_get_handle(RESOURCE_ID_proto_26)));
	text_layer_set_text_color(dogecoin_layer, GColorBlack);
	text_layer_set_text_alignment(dogecoin_layer, GTextAlignmentCenter);
	layer_add_child(window_layer, text_layer_get_layer(dogecoin_layer));
	
	
	time_layer = text_layer_create(GRect(0, 119, 144, 30));
	text_layer_set_font(time_layer,
					   fonts_load_custom_font(resource_get_handle(RESOURCE_ID_proto_24)));
	text_layer_set_text_color(time_layer, GColorBlack);
	text_layer_set_text_alignment(time_layer, GTextAlignmentCenter);
	layer_add_child(window_layer, text_layer_get_layer(time_layer));
	
	date_layer = text_layer_create(GRect(0, 145, 144, 30));
	text_layer_set_font(date_layer,
					   fonts_load_custom_font(resource_get_handle(RESOURCE_ID_proto_14)));
	text_layer_set_text_color(date_layer, GColorBlack);
	text_layer_set_text_alignment(date_layer, GTextAlignmentCenter);
	layer_add_child(window_layer, text_layer_get_layer(date_layer));
	

	image = gbitmap_create_with_resource(RESOURCE_ID_TMB_IMG);
	image_layer = bitmap_layer_create(bounds);
	bitmap_layer_set_bitmap(image_layer, image);
	bitmap_layer_set_alignment(image_layer, GAlignCenter);
	bitmap_layer_set_compositing_mode(image_layer, GCompOpAnd);
	layer_add_child(window_layer, bitmap_layer_get_layer(image_layer));
	
	
	tmbHash = -1;
	tmbScrypt = -1;
	tmbScryptn = -1;
	
	app_message_register_inbox_received(inbox_recieved_callback);
	app_message_open(APP_MESSAGE_INBOX_SIZE_MINIMUM, APP_MESSAGE_OUTBOX_SIZE_MINIMUM);
	
	drawSPD(-2);
	
	tick_timer_service_subscribe(MINUTE_UNIT, drawTime);
	
	time_t init_time = time(0);
	struct tm *init_local = localtime(&init_time);
	
	drawTime(init_local, 0);
	

}

void handle_deinit(void) {
	text_layer_destroy(dogecoin_layer);
	text_layer_destroy(time_layer);
	window_destroy(window);
}


int main(void) {
	handle_init();
	app_event_loop();
	handle_deinit();
}
