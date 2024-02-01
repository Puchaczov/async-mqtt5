#ifndef ASYNC_MQTT5_INTERNAL_TYPES_HPP
#define ASYNC_MQTT5_INTERNAL_TYPES_HPP

#include <optional>
#include <string>
#include <shared_mutex>

#include <async_mqtt5/detail/any_authenticator.hpp>

#include <async_mqtt5/error.hpp>
#include <async_mqtt5/types.hpp>

namespace async_mqtt5::detail {

using byte_citer = std::string::const_iterator;

using time_stamp = std::chrono::time_point<std::chrono::steady_clock>;
using duration = time_stamp::duration;

struct credentials {
	std::string client_id;
	std::optional<std::string> username;
	std::optional<std::string> password;

	credentials() = default;
	credentials(
		std::string client_id,
		std::string username, std::string password
	) :
		client_id(std::move(client_id))
	{
		if (!username.empty())
			this->username = std::move(username);
		if (!password.empty())
			this->password = std::move(password);
	}
};

class session_state {
	uint8_t _flags = 0b00;

	static constexpr uint8_t session_present_flag = 0b01;
	static constexpr uint8_t subscriptions_present_flag = 0b10;
public:
	void session_present(bool present) {
		return update_flag(present, session_present_flag);
	}

	bool session_present() const {
		return _flags & session_present_flag;
	}

	void subscriptions_present(bool present) {
		return update_flag(present, subscriptions_present_flag);
	}

	bool subscriptions_present() const {
		return _flags & subscriptions_present_flag;
	}

private:
	void update_flag(bool set, uint8_t flag) {
		if (set)
			_flags |= flag;
		else
			_flags &= ~flag;
	}
};

struct mqtt_ctx {
	credentials creds;
	std::optional<will> will_msg;
	uint16_t keep_alive = 60;
	connect_props co_props;
	connack_props ca_props;
	session_state state;
	any_authenticator authenticator;
};

struct disconnect_ctx {
	disconnect_rc_e reason_code = disconnect_rc_e::normal_disconnection;
	disconnect_props props = {};
	bool terminal = false;
};

using serial_num_t = uint32_t;
constexpr serial_num_t no_serial = 0;

namespace send_flag {

constexpr unsigned none = 0b000;
constexpr unsigned throttled = 0b001;
constexpr unsigned prioritized = 0b010;
constexpr unsigned terminal = 0b100;

};

} // end namespace async_mqtt5::detail

#endif // !ASYNC_MQTT5_INTERNAL_TYPES_HPP
