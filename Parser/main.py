import requests
import time
from selenium import webdriver
from selenium.webdriver.common.keys import Keys

from order import Order

BOOSTER_SITE = "https://blazingboost.com"
ORDERS_LIST = "https://blazingboost.com/boosters/notassignservices/catid/230-0"
VIEW_DETAILS = "<strong>View Details</strong>"
HREF = 'href="'
START_VALUE = '<td valign="top">'
END_VALUE = '</td>'

headers = {
    "Host": "blazingboost.com",
    "Connection": "keep-alive",
    "Cache-Control": "max-age=0",
    "Upgrade-Insecure-Requests": "1",
    "User-Agent": "Mozilla/5.0 (Windows NT 6.3; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/52.0.2743.116 Safari/537.36",
    "Accept": "text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8",
    "Referer": "https://blazingboost.com/boosters/notassignservices/catid/230-0",
    "Accept-Encoding": "gzip, deflate, sdch, br",
    "Accept-Language": "ru-RU,ru;q=0.8,en-US;q=0.6,en;q=0.4",
}


def getIdFromLink(link):
    start_pos = link.find("/id/")
    end_pos = link.find("/", start_pos + len("/id/"))
    return link[start_pos + len("/id/"): end_pos]


def valueByKeyword(keyword, text, start_srch_substr):
    start_pos = text.find(keyword)
    start_value = text.find(start_srch_substr, start_pos)
    end_value = text.find(END_VALUE, start_value)
    return text[start_value + len(start_srch_substr): end_value]


def getValues(html):
    page_order = Order()
    page_order.setGameClass(valueByKeyword("Service Class:", html, '<td valign="top">'))
    page_order.setRatingFrom(valueByKeyword("From:", html, '<td height="40">'))
    page_order.setRatingTo(valueByKeyword("To:", html, '<td height="40">'))
    page_order.setFraction(valueByKeyword("Service Faction:", html, '<td valign="top">'))
    page_order.setServer(valueByKeyword("Region:", html, '<td height="40">'))
    page_order.setGameMode(valueByKeyword("Game Mode:", html, '<td height="40">'))
    return page_order


def find_orders_links(html_trash):
    start_pos = html_trash.find(VIEW_DETAILS, 0)
    orders_list = []
    while start_pos != -1:
        view_pos = html_trash.find(VIEW_DETAILS, start_pos)
        link_pos = html_trash.rfind(HREF, 0, view_pos)
        link_end_pos = html_trash.find('"', link_pos + len(HREF) + 1)
        link = html_trash[link_pos + len(HREF): link_end_pos]
        orders_list.append(link)
        start_pos = html_trash.find(VIEW_DETAILS, start_pos + len(VIEW_DETAILS))

    return orders_list


def check_order_by_link(browser, link):
    browser.get(link)
    html_trash = browser.page_source


if __name__ == "__main__":
    driver = webdriver.Firefox()
    driver.get(BOOSTER_SITE)
    login_button = driver.find_element_by_id("headerSignInLink")
    login_button.click()
    login_field = driver.find_element_by_id("user_email_id")
    pass_field = driver.find_element_by_id("user_password")
    login_field.send_keys("timbooster")
    pass_field.send_keys("t5qnerf8")
    pass_field.send_keys(Keys.ENTER)
    driver.get(ORDERS_LIST)

    cookies = driver.get_cookies()
    cookie_str = ""
    for cookie in cookies:
        cookie_str += cookie["name"] + "=" + cookie["value"] + "; "
    headers["Cookie"] = cookie_str
    # print cookie_str
    while True:
        page_html = driver.page_source
        links = find_orders_links(page_html)

        for link in links:
            r = requests.get(BOOSTER_SITE + link, headers=headers)
            html_responce = r.text
            parsed_order = getValues(html_responce)
            with open('config', 'r') as config:
                config_lines = config.readlines()
                for config_line in config_lines:
                    config_items = config_line.split()
                    if parsed_order.getGameClass() == config_items[0] and \
                                    parsed_order.getFraction() == config_items[3] and \
                                    parsed_order.getGameServer() == config_items[4] and \
                                    parsed_order.getGameMode() == config_items[5] and \
                                    int(parsed_order.getRatingFrom()) >= int(config_items[1]) and \
                                    int(parsed_order.getRatingTo()) <= int(config_items[2]):
                        order_id = getIdFromLink(link)
                        make_order_button = driver.find_element_by_name("pick_" + order_id)
                        # Uncomment this line when start
                        # make_order_button.click()
                        print "Good order picked: %s %s %s %s %s %s" % (
                        parsed_order.getGameClass(), parsed_order.getFraction(), parsed_order.getRatingFrom(),
                        parsed_order.getRatingTo(), parsed_order.getGameMode(), parsed_order.getGameServer())
                        # print BOOSTER_SITE + link
        time.sleep(5)
