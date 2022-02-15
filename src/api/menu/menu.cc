// Copyright (c) 2012 Intel Corp
// Copyright (c) 2012 The Chromium Authors
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy 
// of this software and associated documentation files (the "Software"), to deal
//  in the Software without restriction, including without limitation the rights
//  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell co
// pies of the Software, and to permit persons to whom the Software is furnished
//  to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in al
// l copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IM
// PLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNES
// S FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS
//  OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WH
// ETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
//  CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#include "content/nw/src/api/menu/menu.h"

#include "base/values.h"
#include "components/zoom/zoom_controller.h"
#include "content/nw/src/api/object_manager.h"
#include "content/nw/src/api/menuitem/menuitem.h"
#include "content/public/browser/web_contents.h"
#include "content/public/common/page_zoom.h"
#include "ui/views/controls/menu/menu_runner.h"
#include "third_party/blink/public/common/page/page_zoom.h"

namespace nw {

Menu::Menu(int id,
           const base::WeakPtr<ObjectManager>& object_manager,
           const base::DictionaryValue& option,
           const std::string& extension_id)
  : Base(id, object_manager, option, extension_id), enable_show_event_(false)  {
  Create(option);
}

Menu::~Menu() {
  Destroy();
}

void Menu::Call(const std::string& method,
                const base::ListValue& arguments,
                content::RenderFrameHost* rvh) {
  if (method == "Append") {
    int object_id = arguments.GetListDeprecated()[0].GetInt();
    Append(object_manager()->GetApiObject<MenuItem>(object_id));
  } else if (method == "Insert") {
    int object_id = arguments.GetListDeprecated()[0].GetInt();
    int pos = arguments.GetListDeprecated()[1].GetInt();
    Insert(object_manager()->GetApiObject<MenuItem>(object_id), pos);
  } else if (method == "Remove") {
    int object_id = arguments.GetListDeprecated()[0].GetInt();
    int pos = arguments.GetListDeprecated()[1].GetInt();
    Remove(object_manager()->GetApiObject<MenuItem>(object_id), pos);
  } else if (method == "Popup") {
    int x = arguments.GetListDeprecated()[0].GetInt();
    int y = arguments.GetListDeprecated()[1].GetInt();
    content::WebContents* web_contents = content::WebContents::FromRenderFrameHost(rvh);
    DCHECK(web_contents);
    zoom::ZoomController* zoom_controller = zoom::ZoomController::FromWebContents(web_contents);

    if (zoom_controller) {
      double zoom_factor = blink::PageZoomLevelToZoomFactor(zoom_controller->GetZoomLevel());
      if (zoom_factor > blink::kMaximumPageZoomFactor) {
        zoom_factor = blink::kMaximumPageZoomFactor;
      }
      if (zoom_factor < blink::kMinimumPageZoomFactor) {
        zoom_factor = blink::kMinimumPageZoomFactor;
      }
      x *= zoom_factor;
      y *= zoom_factor;
    }

    Popup(x, y, rvh);
  } else if (method == "EnableShowEvent") {
    enable_show_event_ = arguments.GetListDeprecated()[0].GetBool();
  } else {
    NOTREACHED() << "Invalid call to Menu method:" << method
                 << " arguments:" << arguments;
  }
}

}  // namespace nw
