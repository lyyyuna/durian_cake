use std::{time::Duration, rc::Rc, thread};
use image;
use reqwest;
use scraper::{Html, Selector};

struct Crawler {
    base: String,
    client: Rc<reqwest::blocking::Client>
}

impl Crawler {
    pub fn new(base: &str) -> Self {
        Crawler {
            base: String::from(base),
            client: Rc::new(reqwest::blocking::Client::new()),      
        }
    }

    pub fn download_image(&self, link: String) -> Result<(), Box<dyn std::error::Error>> {
        let resp = self.client.get(&link).send()?;
        let body = resp.text()?;
        let document = Html::parse_document(&body);
        let selector = Selector::parse("#img img").unwrap();
        let mut img_link_dom = document.select(&selector);
        let img_src = img_link_dom.nth(0).unwrap().value().attr("src").unwrap();

        let mut img_link = String::from(&self.base);
        img_link.push_str(img_src);
        let resp = self.client.get(&img_link).send()?;

        let filename = String::from("img/") + resp.url().path_segments()
                .and_then(|segments| segments.last())
                .and_then(|name| if name.is_empty() {None} else {Some(name)})
                .unwrap_or("default.jpg");

        let content = resp.bytes()?;
        let img = image::load_from_memory(&content)?;
        img.save(filename)?;

        println!("finish download one image: {}", link);
        Ok(())
    }

    pub fn get_page(&self, sub_page: &str) -> Result<(), Box<dyn std::error::Error>> {
        let mut page_link = String::from(&self.base);
        page_link.push_str(sub_page);

        let resp = self.client.get(&page_link).send()?;
        let body = resp.text()?;
        let document = Html::parse_document(&body);
        let selector = Selector::parse("div.slist li a").unwrap();
        let lis = document.select(&selector);

        for li in lis {
            let image_page_link = match li.value().attr("href") {
                Some(href) => {
                    let mut image_link = String::from(&self.base);
                    image_link.push_str(href);
                    image_link
                }
                None => {
                    continue
                }
            };

            println!("image page link: {}", image_page_link);
            self.download_image(image_page_link)?;

            thread::sleep(Duration::from_millis(1000));

            let selector2 = Selector::parse(".page a").unwrap();
            let pages = document.select(&selector2);
            let next_page = pages.last().unwrap();
            let next_page_link = next_page.value().attr("href").unwrap();

            self.get_page(next_page_link)?;
        }
        Ok(())
    }
}

fn main() -> Result<(), Box<dyn std::error::Error>> {
    let c  = Crawler::new("http://pic.netbian.com");
    c.get_page("/4kdongman/index.html")?;
    Ok(())
}
