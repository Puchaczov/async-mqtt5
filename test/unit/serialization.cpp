#include <boost/test/unit_test.hpp>

#include <async_mqtt5/types.hpp>

#include <async_mqtt5/impl/codecs/message_decoders.hpp>
#include <async_mqtt5/impl/codecs/message_encoders.hpp>

using namespace async_mqtt5;
using byte_citer = detail::byte_citer;

BOOST_AUTO_TEST_SUITE(serialization/*, *boost::unit_test::disabled()*/)

BOOST_AUTO_TEST_CASE(test_connect) {
	// testing variables
	// connect
	std::string_view client_id = "async_mqtt_client_id";
	std::string_view uname = "username";
	std::optional<std::string_view> password = std::nullopt;
	uint16_t keep_alive = 60;
	bool clean_start = true;
	// connect properties
	int32_t session_expiry_interval = 29;
	int16_t receive_max = 100;
	int32_t maximum_packet_size = 20000;
	uint16_t topic_alias_max = 1200;
	uint8_t request_response_information = 1;
	uint8_t request_problem_information = 0;
	std::string_view user_property = "user prop";
	std::string_view auth_method = "method";
	std::string_view auth_data = "data";
	// will
	std::string will_topic = "will_topic";
	std::string will_message = "will_message";
	// will properties
	int32_t will_delay_interval = 200;
	uint8_t will_payload_format_indicator = 0;
	int32_t will_message_expiry_interval = 2000;
	std::string_view will_content_type = "will content type";
	std::string_view will_response_topic = "response_topic";
	std::string_view will_correlation_data = "correlation data";
	std::string_view will_user_property = "will prop";

	connect_props cprops;
	cprops[prop::session_expiry_interval] = session_expiry_interval;
	cprops[prop::receive_maximum] = receive_max;
	cprops[prop::maximum_packet_size] = maximum_packet_size;
	cprops[prop::topic_alias_maximum] = topic_alias_max;
	cprops[prop::request_response_information] = request_response_information;
	cprops[prop::request_problem_information] = request_problem_information;
	cprops[prop::user_property].emplace_back(user_property);
	cprops[prop::authentication_method] = auth_method;
	cprops[prop::authentication_data] = auth_data;

	will w { will_topic, will_message };
	w[prop::will_delay_interval] = will_delay_interval;
	w[prop::payload_format_indicator] = will_payload_format_indicator;
	w[prop::message_expiry_interval] = will_message_expiry_interval;
	w[prop::content_type] = will_content_type;
	w[prop::response_topic] = will_response_topic;
	w[prop::correlation_data] = will_correlation_data;
	w[prop::user_property].emplace_back(will_user_property);
	std::optional<will> will_opt { std::move(w) };

	auto msg = encoders::encode_connect(
		client_id, uname, password, keep_alive, clean_start, cprops, will_opt
	);

	byte_citer it = msg.cbegin(), last = msg.cend();
	auto header = decoders::decode_fixed_header(it, last);
	BOOST_CHECK_MESSAGE(header, "Parsing CONNECT fixed header failed.");

	const auto& [control_byte, remain_length] = *header;
	auto rv = decoders::decode_connect(remain_length, it);
	BOOST_CHECK_MESSAGE(rv, "Parsing CONNECT failed.");

	const auto& [client_id_, uname_, password_, keep_alive_, clean_start_, cprops_, w_] = *rv;
	BOOST_CHECK_EQUAL(client_id_, client_id);
	BOOST_CHECK(uname_);
	BOOST_CHECK_EQUAL(*uname_, uname);
	BOOST_CHECK(password_ == password);
	BOOST_CHECK_EQUAL(keep_alive_, keep_alive);
	BOOST_CHECK_EQUAL(clean_start_, clean_start);

	cprops_.visit([](const auto& prop, const auto&) { BOOST_CHECK(prop); return true; });
	BOOST_CHECK_EQUAL(*cprops_[prop::session_expiry_interval], session_expiry_interval);
	BOOST_CHECK_EQUAL(*cprops_[prop::receive_maximum], receive_max);
	BOOST_CHECK_EQUAL(*cprops_[prop::maximum_packet_size], maximum_packet_size);
	BOOST_CHECK_EQUAL(*cprops_[prop::topic_alias_maximum], topic_alias_max);
	BOOST_CHECK_EQUAL(*cprops_[prop::request_response_information], request_response_information);
	BOOST_CHECK_EQUAL(*cprops_[prop::request_problem_information], request_problem_information);
	BOOST_CHECK_EQUAL(cprops_[prop::user_property][0], user_property);
	BOOST_CHECK_EQUAL(*cprops_[prop::authentication_method], auth_method);
	BOOST_CHECK_EQUAL(*cprops_[prop::authentication_data], auth_data);

	BOOST_CHECK(w_);

	BOOST_CHECK_EQUAL((*w_).topic(), will_topic);
	BOOST_CHECK_EQUAL((*w_).message(), will_message);

	(*w_).visit([](const auto& prop, const auto&) { BOOST_CHECK(prop); return true; });
	BOOST_CHECK_EQUAL(*(*w_)[prop::will_delay_interval], will_delay_interval);
	BOOST_CHECK_EQUAL(*(*w_)[prop::payload_format_indicator], will_payload_format_indicator);
	BOOST_CHECK_EQUAL(*(*w_)[prop::message_expiry_interval], will_message_expiry_interval);
	BOOST_CHECK_EQUAL(*(*w_)[prop::content_type], will_content_type);
	BOOST_CHECK_EQUAL(*(*w_)[prop::response_topic], will_response_topic);
	BOOST_CHECK_EQUAL(*(*w_)[prop::correlation_data], will_correlation_data);
	BOOST_CHECK_EQUAL((*w_)[prop::user_property][0], will_user_property);
	
}

BOOST_AUTO_TEST_CASE(test_connack) {
	// testing variables
	uint8_t session_present = 1;
	uint8_t reason_code = reason_codes::server_busy.value();

	int32_t session_expiry_interval = 20;
	int16_t receive_maximum = 2000;
	uint8_t max_qos = 2;
	uint8_t retain_available = 0;
	int32_t maximum_packet_sz = 1024;
	std::string assigned_client_id = "client_id";
	uint16_t topic_alias_max = 128;
	std::string reason_string = "some reason string";
	std::string user_property = "property";
	uint8_t wildcard_sub = 1;
	uint8_t sub_id = 1;
	uint8_t shared_sub = 0;
	int16_t server_keep_alive = 25;
	std::string response_information = "info";
	std::string server_reference = "srv";
	std::string authentication_method = "method";
	std::string authentication_data = "data";

	connack_props cprops;
	cprops[prop::session_expiry_interval] = session_expiry_interval;
	cprops[prop::receive_maximum] = receive_maximum;
	cprops[prop::maximum_qos] = max_qos;
	cprops[prop::retain_available] = retain_available;
	cprops[prop::maximum_packet_size] = maximum_packet_sz;
	cprops[prop::assigned_client_identifier] = assigned_client_id;
	cprops[prop::topic_alias_maximum] = topic_alias_max;
	cprops[prop::reason_string] = reason_string;
	cprops[prop::user_property].push_back(user_property);
	cprops[prop::wildcard_subscription_available] = wildcard_sub;
	cprops[prop::subscription_identifier_available] = sub_id;
	cprops[prop::shared_subscription_available] = shared_sub;
	cprops[prop::server_keep_alive] = server_keep_alive;
	cprops[prop::response_information] = response_information;
	cprops[prop::server_reference] = server_reference;
	cprops[prop::authentication_method] = authentication_method;
	cprops[prop::authentication_data] = authentication_data;

	auto msg = encoders::encode_connack(session_present, reason_code, cprops);

	byte_citer it = msg.cbegin(), last = msg.cend();
	auto header = decoders::decode_fixed_header(it, last);
	BOOST_CHECK_MESSAGE(header, "Parsing CONNACK fixed header failed.");

	const auto& [control_byte, remain_length] = *header;
	auto rv = decoders::decode_connack(remain_length, it);
	BOOST_CHECK_MESSAGE(rv, "Parsing CONNACK failed.");

	const auto& [session_present_, reason_code_, cprops_] = *rv;
	BOOST_CHECK_EQUAL(session_present_, session_present);
	BOOST_CHECK_EQUAL(reason_code_, reason_code);

	cprops_.visit([](const auto& prop, const auto&) { BOOST_CHECK(prop); return true; });
	BOOST_CHECK_EQUAL(*cprops_[prop::session_expiry_interval], session_expiry_interval);
	BOOST_CHECK_EQUAL(*cprops_[prop::receive_maximum], receive_maximum);
	BOOST_CHECK_EQUAL(*cprops_[prop::maximum_qos], max_qos);
	BOOST_CHECK_EQUAL(*cprops_[prop::retain_available], retain_available);
	BOOST_CHECK_EQUAL(*cprops_[prop::maximum_packet_size], maximum_packet_sz);
	BOOST_CHECK_EQUAL(*cprops_[prop::assigned_client_identifier], assigned_client_id);
	BOOST_CHECK_EQUAL(*cprops_[prop::topic_alias_maximum], topic_alias_max);
	BOOST_CHECK_EQUAL(*cprops_[prop::reason_string], reason_string);
	BOOST_CHECK_EQUAL(cprops_[prop::user_property][0], user_property);
	BOOST_CHECK_EQUAL(*cprops_[prop::wildcard_subscription_available], wildcard_sub);
	BOOST_CHECK_EQUAL(*cprops_[prop::subscription_identifier_available], sub_id);
	BOOST_CHECK_EQUAL(*cprops_[prop::shared_subscription_available], shared_sub);
	BOOST_CHECK_EQUAL(*cprops_[prop::server_keep_alive], server_keep_alive);
	BOOST_CHECK_EQUAL(*cprops_[prop::response_information], response_information);
	BOOST_CHECK_EQUAL(*cprops_[prop::server_reference], server_reference);
	BOOST_CHECK_EQUAL(*cprops_[prop::authentication_method], authentication_method);
	BOOST_CHECK_EQUAL(*cprops_[prop::authentication_data], authentication_data);
}

BOOST_AUTO_TEST_CASE(test_publish) {
	// testing variables
	uint16_t packet_id = 31283;
	std::string_view topic = "publish_topic";
	std::string_view payload = "This is some payload I am publishing!";

	uint8_t payload_format_indicator = 1;
	int32_t message_expiry_interval = 70;
	int16_t topic_alias = 16;
	std::string response_topic = "topic/response";
	std::string correlation_data = "correlation data";
	std::string publish_prop_1 = "first publish prop";
	std::string publish_prop_2 = "second publish prop";
	uint32_t subscription_identifier = 123456;
	std::string content_type = "application/octet-stream";

	publish_props pprops;
	pprops[prop::payload_format_indicator] = payload_format_indicator;
	pprops[prop::message_expiry_interval] = message_expiry_interval;
	pprops[prop::topic_alias] = topic_alias;
	pprops[prop::response_topic] = response_topic;
	pprops[prop::correlation_data] = correlation_data;
	pprops[prop::user_property].emplace_back(publish_prop_1);
	pprops[prop::user_property].emplace_back(publish_prop_2);
	pprops[prop::subscription_identifier] = subscription_identifier;
	pprops[prop::content_type] = content_type;

	auto msg = encoders::encode_publish(
		packet_id, topic, payload, 
		qos_e::at_least_once, retain_e::yes, dup_e::no,
		pprops
	);

	byte_citer it = msg.cbegin(), last = msg.cend();
	auto header = decoders::decode_fixed_header(it, last);
	BOOST_CHECK_MESSAGE(header, "Parsing PUBLISH fixed header failed.");

	const auto& [control_byte, remain_length] = *header;
	auto rv = decoders::decode_publish(control_byte, remain_length, it);
	BOOST_CHECK_MESSAGE(rv, "Parsing PUBLISH failed.");

	const auto& [topic_, packet_id_, flags, pprops_, payload_] = *rv;
	BOOST_CHECK(packet_id);
	BOOST_CHECK_EQUAL(*packet_id_, packet_id);
	BOOST_CHECK_EQUAL(topic_, topic);
	BOOST_CHECK_EQUAL(payload_, payload);

	pprops_.visit([](const auto& prop, const auto&) { BOOST_CHECK(prop); return true; });
	BOOST_CHECK_EQUAL(*pprops_[prop::payload_format_indicator], payload_format_indicator);
	BOOST_CHECK_EQUAL(*pprops_[prop::message_expiry_interval], message_expiry_interval);
	BOOST_CHECK_EQUAL(*pprops_[prop::topic_alias], topic_alias);
	BOOST_CHECK_EQUAL(*pprops_[prop::response_topic], response_topic);
	BOOST_CHECK_EQUAL(*pprops_[prop::correlation_data], correlation_data);
	BOOST_CHECK_EQUAL(pprops_[prop::user_property][0], publish_prop_1);
	BOOST_CHECK_EQUAL(pprops_[prop::user_property][1], publish_prop_2);
	BOOST_CHECK_EQUAL(*pprops_[prop::subscription_identifier], subscription_identifier);
	BOOST_CHECK_EQUAL(*pprops_[prop::content_type], content_type);
}

BOOST_AUTO_TEST_CASE(test_large_publish) {
	// testing variables
	uint16_t packet_id = 40001;
	std::string_view topic = "publish_topic";
	std::string large_payload(1'000'000, 'a');

	auto msg = encoders::encode_publish(
		packet_id, topic, large_payload,
		qos_e::at_least_once, retain_e::yes, dup_e::no,
		publish_props {}
	);

	byte_citer it = msg.cbegin(), last = msg.cend();
	auto header = decoders::decode_fixed_header(it, last);
	BOOST_CHECK_MESSAGE(header, "Parsing PUBLISH fixed header failed.");

	const auto& [control_byte, remain_length] = *header;
	auto rv = decoders::decode_publish(control_byte, remain_length, it);
	BOOST_CHECK_MESSAGE(rv, "Parsing PUBLISH failed.");

	const auto& [topic_, packet_id_, flags, pprops, payload_] = *rv;
	BOOST_CHECK(packet_id);
	BOOST_CHECK_EQUAL(*packet_id_, packet_id);
	BOOST_CHECK_EQUAL(topic_, topic);
	BOOST_CHECK_EQUAL(payload_, large_payload);
}

BOOST_AUTO_TEST_CASE(test_puback) {
	// testing variables
	uint16_t packet_id = 9199;
	uint8_t reason_code = 0x93;

	std::string reason_string = "PUBACK reason string";
	std::string user_prop = "PUBACK user prop";

	puback_props pprops;
	pprops[prop::reason_string] = reason_string;
	pprops[prop::user_property].emplace_back(user_prop);

	auto msg = encoders::encode_puback(packet_id, reason_code, pprops);

	byte_citer it = msg.cbegin(), last = msg.cend();
	auto header = decoders::decode_fixed_header(it, last);
	BOOST_CHECK_MESSAGE(header, "Parsing PUBACK fixed header failed.");

	auto packet_id_ = decoders::decode_packet_id(it);
	BOOST_CHECK(packet_id);
	BOOST_CHECK_EQUAL(*packet_id_, packet_id);

	const auto& [control_byte, remain_length] = *header;
	auto rv = decoders::decode_puback(remain_length - sizeof(uint16_t), it);
	BOOST_CHECK_MESSAGE(rv, "Parsing PUBACK failed.");

	const auto& [reason_code_, pprops_] = *rv;
	pprops_.visit([](const auto& prop, const auto&) { BOOST_CHECK(prop); return true; });
	BOOST_CHECK_EQUAL(reason_code_, reason_code);
	BOOST_CHECK_EQUAL(*pprops_[prop::reason_string], reason_string);
	BOOST_CHECK_EQUAL(pprops_[prop::user_property][0], user_prop);
}

BOOST_AUTO_TEST_CASE(test_pubrec) {
	// testing variables
	uint16_t packet_id = 8534;
	uint8_t reason_code = 0x92;

	std::string reason_string = "PUBREC reason string";
	std::string user_prop = "PUBREC user prop";

	pubrec_props pprops;
	pprops[prop::reason_string] = reason_string;
	pprops[prop::user_property].emplace_back(user_prop);

	auto msg = encoders::encode_pubrec(packet_id, reason_code, pprops);

	byte_citer it = msg.cbegin(), last = msg.cend();
	auto header = decoders::decode_fixed_header(it, last);
	BOOST_CHECK_MESSAGE(header, "Parsing PUBREC fixed header failed.");

	auto packet_id_ = decoders::decode_packet_id(it);
	BOOST_CHECK(packet_id);
	BOOST_CHECK_EQUAL(*packet_id_, packet_id);

	const auto& [control_byte, remain_length] = *header;
	auto rv = decoders::decode_pubrec(remain_length - sizeof(uint16_t), it);
	BOOST_CHECK_MESSAGE(rv, "Parsing PUBREC failed.");

	const auto& [reason_code_, pprops_] = *rv;
	pprops_.visit([](const auto& prop, const auto&) { BOOST_CHECK(prop); return true; });
	BOOST_CHECK_EQUAL(reason_code_, reason_code);
	BOOST_CHECK_EQUAL(*pprops_[prop::reason_string], reason_string);
	BOOST_CHECK_EQUAL(pprops_[prop::user_property][0], user_prop);
}

BOOST_AUTO_TEST_CASE(test_pubrel) {
	// testing variables
	uint16_t packet_id = 21455;
	uint8_t reason_code = 0x00;

	std::string reason_string = "PUBREL reason string";
	std::string user_prop = "PUBREL user prop";

	pubrel_props pprops;
	pprops[prop::reason_string] = reason_string;
	pprops[prop::user_property].emplace_back(user_prop);

	auto msg = encoders::encode_pubrel(packet_id, reason_code, pprops);

	byte_citer it = msg.cbegin(), last = msg.cend();
	auto header = decoders::decode_fixed_header(it, last);
	BOOST_CHECK_MESSAGE(header, "Parsing PUBREL fixed header failed.");

	auto packet_id_ = decoders::decode_packet_id(it);
	BOOST_CHECK(packet_id);
	BOOST_CHECK_EQUAL(*packet_id_, packet_id);

	const auto& [control_byte, remain_length] = *header;
	auto rv = decoders::decode_pubrel(remain_length - sizeof(uint16_t), it);
	BOOST_CHECK_MESSAGE(rv, "Parsing PUBREL failed.");

	const auto& [reason_code_, pprops_] = *rv;
	pprops_.visit([](const auto& prop, const auto&) { BOOST_CHECK(prop); return true; });
	BOOST_CHECK_EQUAL(reason_code_, reason_code);
	BOOST_CHECK_EQUAL(*pprops_[prop::reason_string], reason_string);
	BOOST_CHECK_EQUAL(pprops_[prop::user_property][0], user_prop);
}

BOOST_AUTO_TEST_CASE(test_pubcomp) {
	// testing variables
	uint16_t packet_id = 21455;
	uint8_t reason_code = 0x00;

	std::string reason_string = "PUBCOMP reason string";
	std::string user_prop = "PUBCOMP user prop";

	pubcomp_props pprops;
	pprops[prop::reason_string] = reason_string;
	pprops[prop::user_property].emplace_back(user_prop);

	auto msg = encoders::encode_pubcomp(packet_id, reason_code, pprops);

	byte_citer it = msg.cbegin(), last = msg.cend();
	auto header = decoders::decode_fixed_header(it, last);
	BOOST_CHECK_MESSAGE(header, "Parsing PUBCOMP fixed header failed.");

	auto packet_id_ = decoders::decode_packet_id(it);
	BOOST_CHECK(packet_id);
	BOOST_CHECK_EQUAL(*packet_id_, packet_id);

	const auto& [control_byte, remain_length] = *header;
	auto rv = decoders::decode_pubcomp(remain_length - sizeof(uint16_t), it);
	BOOST_CHECK_MESSAGE(rv, "Parsing PUBCOMP failed.");

	const auto& [reason_code_, pprops_] = *rv;
	pprops_.visit([](const auto& prop, const auto&) { BOOST_CHECK(prop); return true; });
	BOOST_CHECK_EQUAL(reason_code_, reason_code);
	BOOST_CHECK_EQUAL(*pprops_[prop::reason_string], reason_string);
	BOOST_CHECK_EQUAL(pprops_[prop::user_property][0], user_prop);
}

BOOST_AUTO_TEST_CASE(test_subscribe) {
	//testing variables
	uint32_t sub_id = 1'234'567;
	std::string user_prop = "SUBSCRIBE user prop";

	// delete using lines when we shorten the enum names
	using no_local_e = subscribe_options::no_local_e;
	using retain_as_published_e = subscribe_options::retain_as_published_e;
	using retain_handling_e = subscribe_options::retain_handling_e;

	qos_e qos = qos_e::at_least_once;
	no_local_e no_local = no_local_e::yes;
	retain_as_published_e retain_as_published = retain_as_published_e::retain;
	retain_handling_e retain_handling = retain_handling_e::not_send;

	subscribe_props sprops;
	sprops[prop::subscription_identifier] = sub_id;
	sprops[prop::user_property].push_back(user_prop);

	std::vector<subscribe_topic> filters {
		{
			"subscribe topic",
			{ qos, no_local, retain_as_published, retain_handling }
		}
	};
	uint16_t packet_id = 65535;

	auto msg = encoders::encode_subscribe(packet_id, filters, sprops);

	byte_citer it = msg.cbegin(), last = msg.cend();
	auto header = decoders::decode_fixed_header(it, last);
	BOOST_CHECK_MESSAGE(header, "Parsing SUBSCRIBE fixed header failed.");

	const auto& [control_byte, remain_length] = *header;
	auto packet_id_ = decoders::decode_packet_id(it);
	BOOST_CHECK(packet_id);
	BOOST_CHECK_EQUAL(*packet_id_, packet_id);
	auto rv = decoders::decode_subscribe(remain_length - sizeof(uint16_t), it);
	BOOST_CHECK_MESSAGE(rv, "Parsing SUBSCRIBE failed.");

	const auto& [sprops_, filters_] = *rv;
	const auto& filter_ = filters_[0];
	BOOST_CHECK_EQUAL(std::get<0>(filter_), filters[0].topic_filter);

	uint8_t options_ = std::get<1>(filter_);
	uint8_t mask = (static_cast<uint8_t>(retain_handling) << 4) |
		(static_cast<uint8_t>(retain_as_published) << 3) |
		(static_cast<uint8_t>(no_local) << 2) |
		static_cast<uint8_t>(qos);
	BOOST_CHECK_EQUAL(options_, mask);

	sprops_.visit([](const auto& prop, const auto&) { BOOST_CHECK(prop); return true; });
	BOOST_CHECK_EQUAL(*sprops_[prop::subscription_identifier], sub_id);
	BOOST_CHECK_EQUAL(sprops_[prop::user_property][0], user_prop);
}

BOOST_AUTO_TEST_CASE(test_suback) {
	//testing variables
	uint16_t packet_id = 142;
	std::vector<uint8_t> reason_codes { 48, 28 };

	std::string reason_string = "subscription accepted";
	std::string user_prop = "SUBACK user prop";

	suback_props sprops;
	sprops[prop::reason_string] = reason_string;
	sprops[prop::user_property].push_back(user_prop);

	auto msg = encoders::encode_suback(packet_id, reason_codes, sprops);

	byte_citer it = msg.cbegin(), last = msg.cend();
	auto header = decoders::decode_fixed_header(it, last);
	BOOST_CHECK_MESSAGE(header, "Parsing SUBACK fixed header failed.");

	const auto& [control_byte, remain_length] = *header;
	auto packet_id_ = decoders::decode_packet_id(it);
	BOOST_CHECK(packet_id);
	BOOST_CHECK_EQUAL(*packet_id_, packet_id);
	auto rv = decoders::decode_suback(remain_length - sizeof(uint16_t), it);
	BOOST_CHECK_MESSAGE(rv, "Parsing SUBACK failed.");

	const auto& [sprops_, reason_codes_] = *rv;
	BOOST_CHECK(reason_codes_ == reason_codes);

	sprops_.visit([](const auto& prop, const auto&) { BOOST_CHECK(prop); return true; });
	BOOST_CHECK_EQUAL(*sprops_[prop::reason_string], reason_string);
	BOOST_CHECK_EQUAL(sprops_[prop::user_property][0], user_prop);
}

BOOST_AUTO_TEST_CASE(test_unsubscribe) {
	// testing variables
	uint16_t packet_id = 14423;
	std::vector<std::string> topics { "first topic", "second/topic" };

	std::string user_prop = "UNSUBSCRIBE user prop";

	unsubscribe_props uprops;
	uprops[prop::user_property].push_back(user_prop);

	auto msg = encoders::encode_unsubscribe(packet_id, topics, uprops);

	byte_citer it = msg.cbegin(), last = msg.cend();
	auto header = decoders::decode_fixed_header(it, last);
	BOOST_CHECK_MESSAGE(header, "Parsing UNSUBSCRIBE fixed header failed.");

	const auto& [control_byte, remain_length] = *header;
	auto packet_id_ = decoders::decode_packet_id(it);
	BOOST_CHECK(packet_id);
	BOOST_CHECK_EQUAL(*packet_id_, packet_id);
	auto rv = decoders::decode_unsubscribe(remain_length - sizeof(uint16_t), it);
	BOOST_CHECK_MESSAGE(rv, "Parsing UNSUBSCRIBE failed.");

	const auto& [uprops_, topics_] = *rv;
	BOOST_CHECK(topics_ == topics);

	uprops_.visit([](const auto& prop, const auto&) { BOOST_CHECK(prop); return true; });
	BOOST_CHECK_EQUAL(uprops_[prop::user_property][0], user_prop);
}

BOOST_AUTO_TEST_CASE(test_unsuback) {
	// testing variables
	uint16_t packet_id = 42;
	std::vector<uint8_t> reason_codes{ 48, 28 };

	std::string reason_string = "some unsuback reason string";
	std::string user_prop = "UNSUBACK user prop";

	unsuback_props uprops;
	uprops[prop::reason_string] = reason_string;
	uprops[prop::user_property].push_back(user_prop);

	auto msg = encoders::encode_unsuback(packet_id, reason_codes, uprops);

	byte_citer it = msg.cbegin(), last = msg.cend();
	auto header = decoders::decode_fixed_header(it, last);
	BOOST_CHECK_MESSAGE(header, "Parsing UNSUBACK fixed header failed.");

	const auto& [control_byte, remain_length] = *header;
	auto packet_id_ = decoders::decode_packet_id(it);
	BOOST_CHECK(packet_id);
	BOOST_CHECK_EQUAL(*packet_id_, packet_id);
	auto rv = decoders::decode_unsuback(remain_length - sizeof(uint16_t), it);
	BOOST_CHECK_MESSAGE(header, "Parsing UNSUBACK failed.");

	const auto& [uprops_, reason_codes_] = *rv;
	BOOST_CHECK(reason_codes_ == reason_codes);

	uprops_.visit([](const auto& prop, const auto&) { BOOST_CHECK(prop); return true; });
	BOOST_CHECK_EQUAL(*uprops_[prop::reason_string], reason_string);
	BOOST_CHECK_EQUAL(uprops_[prop::user_property][0], user_prop);
}

BOOST_AUTO_TEST_CASE(test_disconnect) {
	// testing variables
	uint8_t reason_code = 0x04;

	int32_t session_expiry_interval = 50;
	std::string reason_string = "a reason";
	std::string user_property = "DISCONNECT user property";
	std::string server_reference = "server";

	disconnect_props dprops;
	dprops[prop::session_expiry_interval] = session_expiry_interval;
	dprops[prop::reason_string] = reason_string;
	dprops[prop::user_property].emplace_back(user_property);
	dprops[prop::server_reference] = server_reference;

	auto msg = encoders::encode_disconnect(reason_code, dprops);

	byte_citer it = msg.cbegin(), last = msg.cend();
	auto header = decoders::decode_fixed_header(it, last);
	BOOST_CHECK_MESSAGE(header, "Parsing DISCONNECT fixed header failed.");

	const auto& [control_byte, remain_length] = *header;
	auto rv = decoders::decode_disconnect(remain_length, it);
	BOOST_CHECK_MESSAGE(header, "Parsing DISCONNECT failed.");

	const auto& [reason_code_, dprops_] = *rv;
	BOOST_CHECK_EQUAL(reason_code_, reason_code);

	dprops_.visit([](const auto& prop, const auto&) { BOOST_CHECK(prop); return true; });
	BOOST_CHECK_EQUAL(*dprops_[prop::session_expiry_interval], session_expiry_interval);
	BOOST_CHECK_EQUAL(*dprops_[prop::reason_string], reason_string);
	BOOST_CHECK_EQUAL(dprops_[prop::user_property][0], user_property);
	BOOST_CHECK_EQUAL(*dprops_[prop::server_reference], server_reference);
}

BOOST_AUTO_TEST_CASE(test_auth) {
	// testing variables
	uint8_t reason_code = 0x18;

	std::string authentication_method = "method";
	std::string authentication_data = "data";

	std::string reason_string = "reason";
	std::string user_property = "AUTH user propety";

	auth_props aprops;
	aprops[prop::authentication_method] = authentication_method;
	aprops[prop::authentication_data] = authentication_data;
	aprops[prop::reason_string] = reason_string;
	aprops[prop::user_property].emplace_back(user_property);

	auto msg = encoders::encode_auth(reason_code, aprops);

	byte_citer it = msg.cbegin(), last = msg.cend();
	auto header = decoders::decode_fixed_header(it, last);
	BOOST_CHECK_MESSAGE(header, "Parsing AUTH fixed header failed.");

	const auto& [control_byte, remain_length] = *header;
	auto rv = decoders::decode_auth(remain_length, it);
	BOOST_CHECK_MESSAGE(rv, "Parsing AUTH failed.");

	const auto& [reason_code_, aprops_] = *rv;
	BOOST_CHECK_EQUAL(reason_code_, reason_code);

	aprops_.visit([](const auto& prop, const auto&) { BOOST_CHECK(prop); return true; });
	BOOST_CHECK_EQUAL(*aprops_[prop::authentication_method], authentication_method);
	BOOST_CHECK_EQUAL(*aprops_[prop::authentication_data], authentication_data);
	BOOST_CHECK_EQUAL(*aprops_[prop::reason_string], reason_string);
	BOOST_CHECK_EQUAL(aprops_[prop::user_property][0], user_property);
}

BOOST_AUTO_TEST_CASE(test_pingreq) {
	auto msg = encoders::encode_pingreq();

	auto encoded_pingreq = std::string({ -64 /* 192 */, 0 });
	BOOST_CHECK_EQUAL(msg, encoded_pingreq);
}

BOOST_AUTO_TEST_CASE(test_pingresp) {
	auto msg = encoders::encode_pingresp();

	auto encoded_pingresp = std::string({ -48 /* 208 */, 0 });
	BOOST_CHECK_EQUAL(msg, encoded_pingresp);
}

BOOST_AUTO_TEST_SUITE_END()