#include <boost/test/unit_test.hpp>

#include <sstream>
#include <vector>

#include <async_mqtt5/error.hpp>

using namespace async_mqtt5;

BOOST_AUTO_TEST_SUITE(error/*, *boost::unit_test::disabled()*/)

BOOST_AUTO_TEST_CASE(client_ec_to_string) {
	// Ensure that all branches of the switch/case are covered

	std::vector<client::error> ecs = {
		client::error::malformed_packet,
		client::error::packet_too_large,
		client::error::session_expired,
		client::error::pid_overrun,
		client::error::invalid_topic,
		client::error::qos_not_supported,
		client::error::retain_not_available,
		client::error::topic_alias_maximum_reached,
		client::error::wildcard_subscription_not_available,
		client::error::subscription_identifier_not_available,
		client::error::shared_subscription_not_available
	};

	const client::client_ec_category& cat = client::get_error_code_category();
	BOOST_CHECK_NO_THROW(cat.name());

	constexpr auto default_output = "Unknown client error.";
	for (auto ec : ecs)
		BOOST_CHECK(cat.message(static_cast<int>(ec)) != default_output);

	// default branch
	BOOST_CHECK(cat.message(1) == default_output);
}


BOOST_AUTO_TEST_CASE(reason_code_to_string) {
	// Ensure that all branches of the switch/case are covered
	using namespace reason_codes;

	std::vector<reason_code> rcs = {
		empty, success, normal_disconnection,
		granted_qos_0, granted_qos_1, granted_qos_2,
		disconnect_with_will_message, no_matching_subscribers,
		no_subscription_existed, continue_authentication, reauthenticate,
		unspecified_error, malformed_packet, protocol_error,
		implementation_specific_error, unsupported_protocol_version,
		client_id_not_valid,bad_username_or_password,
		not_authorized, server_unavailable, server_busy, banned,
		server_shutting_down, bad_authentication_method, keep_alive_timeout,
		session_taken_over, topic_filter_invalid,topic_name_invalid,
		packet_id_in_use, packet_id_not_found, receive_maximum_exceeded,
		topic_alias_invalid, packet_too_large, message_rate_too_high,
		quota_exceeded, administrative_action, payload_format_invalid,
		retain_not_supported, qos_not_supported, use_another_server,
		server_moved, shared_subscriptions_not_supported, connection_rate_exceeded,
		maximum_connect_time, subscription_ids_not_supported,
		wildcard_subscriptions_not_supported
	};

	BOOST_CHECK_EQUAL(rcs.size(), 46);

	constexpr auto default_output = "Invalid reason code";
	for (const auto& rc: rcs) 
		BOOST_CHECK(rc.message() != "Invalid reason code");

	// default branch
	BOOST_CHECK(
		reason_code(0x05, reason_codes::category::suback).message() == default_output
	);
}

BOOST_AUTO_TEST_CASE(reason_code_to_stream) {
	std::ostringstream stream;
	stream << reason_codes::success;
	BOOST_CHECK_EQUAL(stream.str(), reason_codes::success.message());
}

BOOST_AUTO_TEST_SUITE_END();