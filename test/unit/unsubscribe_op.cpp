#include <boost/test/unit_test.hpp>

#include <boost/asio/io_context.hpp>

#include <async_mqtt5/impl/unsubscribe_op.hpp>

#include "test_common/test_service.hpp"

using namespace async_mqtt5;

BOOST_AUTO_TEST_SUITE(unsubscribe_op/*, *boost::unit_test::disabled()*/)

BOOST_AUTO_TEST_CASE(pid_overrun) {
	constexpr int expected_handlers_called = 1;
	int handlers_called = 0;

	asio::io_context ioc;
	using client_service_type = test::overrun_client<asio::ip::tcp::socket>;
	auto svc_ptr = std::make_shared<client_service_type>(ioc.get_executor(), "");

	auto handler = [&handlers_called](error_code ec, std::vector<reason_code> rcs, unsuback_props) {
		++handlers_called;
		BOOST_CHECK(ec == client::error::pid_overrun);
		BOOST_ASSERT(rcs.size() == 1);
		BOOST_CHECK_EQUAL(rcs[0], reason_codes::empty);
	};

	detail::unsubscribe_op<
		client_service_type, decltype(handler)
	> { svc_ptr, std::move(handler) }
	.perform({ "topic" }, unsubscribe_props {});

	ioc.run_for(std::chrono::milliseconds(500));
	BOOST_CHECK_EQUAL(handlers_called, expected_handlers_called);
}

void run_test(
	error_code expected_ec, const std::vector<std::string>& topics,
	const unsubscribe_props& uprops = {}, const connack_props& cprops = {}
) {
	constexpr int expected_handlers_called = 1;
	int handlers_called = 0;

	asio::io_context ioc;
	using client_service_type = test::test_service<asio::ip::tcp::socket>;
	auto svc_ptr = std::make_shared<client_service_type>(ioc.get_executor(), cprops);

	auto handler = [&handlers_called, expected_ec, num_tp = topics.size()]
	(error_code ec, std::vector<reason_code> rcs, unsuback_props) {
		++handlers_called;

		BOOST_CHECK(ec == expected_ec);
		BOOST_ASSERT(rcs.size() == num_tp);

		for (size_t i = 0; i < rcs.size(); ++i)
			BOOST_CHECK_EQUAL(rcs[i], reason_codes::empty);
	};

	detail::unsubscribe_op<
		client_service_type, decltype(handler)
	> { svc_ptr, std::move(handler) }
	.perform(topics, uprops);

	ioc.run_for(std::chrono::milliseconds(500));
	BOOST_CHECK_EQUAL(handlers_called, expected_handlers_called);
}

void run_test(
	error_code expected_ec, const std::string& topic,
	const unsubscribe_props& uprops = {}, const connack_props& cprops = {}
) {
	return run_test(
		expected_ec, std::vector<std::string> { topic }, uprops, cprops
	);
}

BOOST_AUTO_TEST_CASE(invalid_topic_filter_1) {
	run_test(client::error::invalid_topic, "");
}

BOOST_AUTO_TEST_CASE(invalid_topic_filter_2) {
	run_test(client::error::invalid_topic, "+topic");
}

BOOST_AUTO_TEST_CASE(invalid_topic_filter_3) {
	run_test(client::error::invalid_topic, "topic+");
}

BOOST_AUTO_TEST_CASE(invalid_topic_filter_4) {
	run_test(client::error::invalid_topic, "#topic");
}

BOOST_AUTO_TEST_CASE(invalid_topic_filter_5) {
	run_test(client::error::invalid_topic, "some/#/topic");
}

BOOST_AUTO_TEST_CASE(invalid_topic_filter_6) {
	run_test(client::error::invalid_topic, "some/topic#");
}

BOOST_AUTO_TEST_CASE(malformed_user_property_1) {
	unsubscribe_props uprops;
	uprops[prop::user_property].push_back(std::string(10, char(0x01)));

	run_test(client::error::malformed_packet, "topic", uprops);
}

BOOST_AUTO_TEST_CASE(malformed_user_property_2) {
	unsubscribe_props uprops;
	uprops[prop::user_property].push_back(std::string(75000, 'a'));

	run_test(client::error::malformed_packet, "topic", uprops);
}

BOOST_AUTO_TEST_CASE(packet_too_large) {
	connack_props cprops;
	cprops[prop::maximum_packet_size] = 10;

	run_test(
		client::error::packet_too_large, "very large topic",
		unsubscribe_props {}, cprops
	);
}

BOOST_AUTO_TEST_CASE(zero_topic_filters) {
	run_test(client::error::invalid_topic, std::vector<std::string> {});
}

BOOST_AUTO_TEST_SUITE_END()
