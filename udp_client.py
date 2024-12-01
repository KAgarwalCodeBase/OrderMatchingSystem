import socket
import time

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
        except Exception as e:
            print(f"Error while sending order: {e}")


def validate_order(order):
    """Validate the order format before sending."""
    try:
        fields = order.split()
        if len(fields) != 7:
            raise ValueError("Order must contain exactly 7 fields.")
        order_id, side, price, quantity, timestamp, trader_id, is_market_order = fields
        if side not in ['B', 'S']:
            raise ValueError("Side must be 'B' (Buy) or 'S' (Sell).")
        float(price)  # Price should be a valid float
        int(quantity)  # Quantity should be a valid integer
        int(timestamp)  # Timestamp should be a valid integer
        int(trader_id)  # Trader ID should be a valid integer
        if is_market_order not in ['0', '1']:
            raise ValueError("isMarketOrder must be '0' (False) or '1' (True).")
        return True
    except ValueError as e:
        print(f"Invalid order: {e}")
        return False


# Test Cases
def run_test_cases():
    """Run a set of test cases to validate the matching engine."""
    print("\n--- Running Test Cases ---\n")

    test_orders = [
        # Valid Orders
        "1 B 100.50 10 169348127 2001 0",  # Buy limit order
        "2 S 100.50 5 169348128 2002 0",   # Sell limit order (partial match expected)
        "3 S 101.00 7 169348129 2003 0",   # Sell limit order (unmatched)
        "4 B 101.00 5 169348130 2004 1",   # Buy market order (matches higher price)

        # Invalid Orders
        "5 X 100.50 10 169348131 2005 0",  # Invalid side
        "6 B -50.00 10 169348132 2006 0",  # Negative price
        "7 S 100.50 abc 169348133 2007 0", # Non-numeric quantity
        "8 B 100.50 10 169348134 2008 2",  # Invalid isMarketOrder
    ]

    for order in test_orders:
        print(f"Testing order: {order}")
        if validate_order(order):
            send_order(order)
        else:
            print("Order validation failed. Skipping...\n")

    print("\n--- Test Cases Completed ---")


# Main Function
if __name__ == "__main__":
    print("UDP Client for Low Latency Order Matching System\n")
    
    # Run Test Cases
    run_test_cases()

    # Optional: Manual Input
    while True:
        user_order = input("\nEnter an order (or type 'exit' to quit): ")
        if user_order.lower() == "exit":
            break
        if validate_order(user_order):
            send_order(user_order)
        else:
            print("Invalid order format. Please try again.")
