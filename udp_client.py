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
        "3 B 100.50 0 169348127 2001 0",   # Zero quantity
        "4 S 100.123456789 10 169348128 2002 0", # High precision price
        "5 B 100.50 10 0 2003 0",          # Very old timestamp
        "6 S 100.50 10 9999999999 2004 0", # Far-future timestamp
        "7 S 100.50 5 169348129 2005 0",   # FIFO matching test
        "8 S 100.50 5 169348130 2006 0",   # FIFO matching test
        "9 B 100.50 10 169348131 2007 0",  # FIFO matching test
    ]


def run_test_cases(output_file):
    """Run a set of test cases to validate the matching engine."""
    print("\n--- Running Test Cases ---\n")
    test_orders = generate_test_cases()

    with open(output_file, "w") as f:
        for order in test_orders:
            print(f"Testing order: {order}")
            response = send_order(order)
            f.write(f"Order: {order}\nResponse: {response}\n\n")

    print("\n--- Test Cases Completed ---")


# Main Function
if __name__ == "__main__":
    print("UDP Client for Low Latency Order Matching System\n")

    output_file = "test_results.txt"  # File to store results
    run_test_cases(output_file)

    print(f"Test results have been written to {output_file}.")
