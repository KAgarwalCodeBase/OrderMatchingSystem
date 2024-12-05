import socket

# UDP client setup
SERVER_ADDRESS = ("127.0.0.1", 8080)  # Server IP and port
BUFFER_SIZE = 1024

# Order format: <orderId> <side> <price> <quantity> <timestamp> <traderId> <isMarketOrder>
def send_order(order):
    """Send an order to the matching engine and receive a response."""
    with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as sock:
        try:
            # Send order to server
            sock.sendto(order.encode(), SERVER_ADDRESS)
            print(f"Sent order: {order}")

            # Receive response from server
            response, _ = sock.recvfrom(BUFFER_SIZE)
            print(f"Server response: {response.decode()}")
            return response.decode()
        except Exception as e:
            print(f"Error while sending order: {e}")
            return f"Error: {e}"


# Test Cases
def generate_test_cases():
    """Generate test cases covering all scenarios."""
    return [
        # Valid Orders
        "1 B 100.50 10 169348127 2001 0",  # Exact match (Buy)
        "2 S 100.50 10 169348128 2002 0",  # Exact match (Sell)

        # Edge Cases
        "3 B 100.50 0 169348127 2001 2",   # Zero quantity
        "4 S 100.123456789 10 169348128 2002 0", # High precision price
        "5 B 100.50 10 0 2003 0",          # Very old timestamp
        "6 S 100.50 10 9999999999 2004 0", # Far-future timestamp
        "7 S 100.50 5 169348129 2005 0",   # FIFO matching test
        "8 S 100.50 5 169348130 2006 0",   # FIFO matching test
        "9 B 100.50 10 169348131 2007 0",  # FIFO matching test

        # Valid Orders
        "10 B 101.00 15 169348132 2008 0",  # Valid buy order
        "11 S 99.50 20 169348133 2009 0",   # Valid sell order
        "12 B 100.00 5 169348134 2010 1",   # Market buy order
        "13 S 100.00 5 169348135 2011 1",   # Market sell order

        # Invalid Orders
        "14 B -100.00 10 169348136 2012 0",  # Negative price
        "15 S 100.00 -10 169348137 2013 0",  # Negative quantity
        "16 B 100.00 10 169348138 2014 2",   # Invalid isMarketOrder value
        "17 S 100.00 10 -169348139 2015 0",  # Negative timestamp
        "18 B",                              # Incomplete order
        "19 B 100.50 5 169348140 2016",      # Missing isMarketOrder field

        # Edge Cases
        "20 B 100.00 0 169348141 2017 0",    # Zero quantity (edge case)
        "21 S 0.00 10 169348142 2018 0",     # Zero price (edge case)
        "22 B 100.123456789 1 169348143 2019 0",  # High-precision price
        "23 S 9999999999.99 10 169348144 2020 0", # Very large price
        "24 B 100.00 999999 169348145 2021 0",    # Very large quantity
    ]


def run_test_cases():
    """Run a set of test cases to validate the matching engine."""
    print("\n--- Running Test Cases ---\n")
    test_orders = generate_test_cases()

    for order in test_orders:
        print(f"Testing order: {order}")
        send_order(order)

    print("\n--- Test Cases Completed ---")


def interactive_mode():
    """Allow the user to place additional orders interactively."""
    print("\n--- Enter Interactive Mode ---")
    print("Type your orders in the format: <orderId> <side> <price> <quantity> <timestamp> <traderId> <isMarketOrder>")
    print("Type 'exit' to quit and shut down the server.\n")

    while True:
        try:
            user_order = input("Enter order: ")
            if user_order.lower() == "exit":
                print("Shutting down the server. Goodbye!")
                send_order("shutdown")  # Send the shutdown command to the server
                break
            send_order(user_order)
        except KeyboardInterrupt:
            # Handle Ctrl+C gracefully
            print("\nExiting interactive mode. Goodbye!")
            break
        except Exception as e:
            print(f"An error occurred: {e}")

# Main Function
if __name__ == "__main__":
    print("UDP Client for Low Latency Order Matching System\n")

    # Run the predefined test cases
    run_test_cases()

    # Enter interactive mode for additional orders
    interactive_mode()
