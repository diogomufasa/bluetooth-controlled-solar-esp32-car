import bluetooth
import time
from machine import Pin, PWM

# --- Pin Definitions ---
r_pwm = PWM(Pin(33), freq=1000)
l_pwm = PWM(Pin(32), freq=1000)
RF = Pin(13, Pin.OUT)
LF = Pin(26, Pin.OUT)
RB = Pin(12, Pin.OUT)
LB = Pin(27, Pin.OUT)
trig = Pin(5, Pin.OUT)
echo = Pin(18, Pin.IN)

SOUND_SPEED = 0.034
speed = 150

# --- Helpers ---

def set_pwm(l, r):
    l_pwm.duty(l)
    r_pwm.duty(r)

def stop():
    RF.off(); LB.off(); LF.off(); RB.off()
    set_pwm(0, 0)

def cal_distance():
    trig.off()
    time.sleep_us(2)
    trig.on()
    time.sleep_us(10)
    trig.off()
    while echo.value() == 0:
        pass
    t_start = time.ticks_us()
    while echo.value() == 1:
        pass
    t_end = time.ticks_us()
    duration = time.ticks_diff(t_end, t_start)
    return (duration * SOUND_SPEED) / 2

def forward(dis):
    if dis > 30:
        RF.on(); LB.off(); LF.on(); RB.off()
        set_pwm(speed, speed)
    else:
        stop()

def backward():
    RF.off(); LB.on(); LF.off(); RB.on()
    set_pwm(speed, speed)

def left():
    RF.on(); LB.off(); LF.on(); RB.off()
    set_pwm(150, 255)

def right():
    RF.on(); LB.off(); LF.on(); RB.off()
    set_pwm(255, 150)

def forward_left():
    RF.on(); LB.off(); LF.on(); RB.off()
    set_pwm(150, 255)

def forward_right():
    RF.on(); LB.off(); LF.on(); RB.off()
    set_pwm(255, 190)

def back_left():
    RF.on(); LB.on(); LF.off(); RB.off()
    set_pwm(speed, speed)

def back_right():
    RF.off(); LB.off(); LF.on(); RB.on()
    set_pwm(speed, speed)

def auto_avoid(dis):
    # If obstacle detected, reverse automatically
    if dis <= 30:
        RF.off(); LB.off(); LF.on(); RB.on()
        set_pwm(speed, speed)

# --- Bluetooth Setup ---
bt = bluetooth.BLE()
bt.active(True)

# Advertise as "roboS"
name = b'roboS'
adv_data = bytearray([0x02, 0x01, 0x06, len(name) + 1, 0x09]) + name
bt.gap_advertise(100, adv_data=adv_data)

_conn_handle = None
_rx_data = bytearray()

UART_SERVICE_UUID = bluetooth.UUID("6E400001-B5B3-F393-E0A9-E50E24DCCA9E")
UART_RX_UUID      = bluetooth.UUID("6E400002-B5B3-F393-E0A9-E50E24DCCA9E")
UART_TX_UUID      = bluetooth.UUID("6E400003-B5B3-F393-E0A9-E50E24DCCA9E")

UART_SERVICE = (
    UART_SERVICE_UUID,
    (
        (UART_TX_UUID, bluetooth.FLAG_NOTIFY),
        (UART_RX_UUID, bluetooth.FLAG_WRITE),
    ),
)

((tx, rx),) = bt.gatts_register_services((UART_SERVICE,))

def bt_irq(event, data):
    global _conn_handle, _rx_data
    if event == 1:  # _IRQ_CENTRAL_CONNECT
        _conn_handle, _, _ = data
    elif event == 2:  # _IRQ_CENTRAL_DISCONNECT
        _conn_handle = None
        bt.gap_advertise(100, adv_data=adv_data)
    elif event == 3:  # _IRQ_GATTS_WRITE
        _rx_data = bt.gatts_read(rx)

bt.irq(bt_irq)

# Speed command map
speed_map = {
    b'1': 150, b'2': 160, b'3': 170, b'4': 180,
    b'5': 190, b'6': 200, b'7': 210, b'8': 220,
    b'9': 235, b'q': 255
}

# --- Main Loop ---
while True:
    dis = cal_distance()
    auto_avoid(dis)

    if _rx_data:
        cmd = _rx_data
        _rx_data = bytearray()

        if cmd == b'S':
            stop()
        elif cmd == b'F':
            forward(dis)
        elif cmd == b'B':
            backward()
        elif cmd == b'L':
            left()
        elif cmd == b'R':
            right()
        elif cmd == b'G':
            forward_left()
        elif cmd == b'I':
            forward_right()
        elif cmd == b'H':
            back_left()
        elif cmd == b'J':
            back_right()
        elif cmd in speed_map:
            speed = speed_map[cmd]

    time.sleep_ms(10)
