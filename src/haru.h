#ifndef HARU_H
#define HARU_H

#include <godot_cpp/classes/control.hpp>
#include <godot_cpp/core/object.hpp>
#include <godot_cpp/core/binder_common.hpp>

#include <libharu/include/hpdf.h>

using namespace godot;

inline void hdpf_error_handler(HPDF_STATUS error_no, HPDF_STATUS detail_no, void *user_data)
{
    printf("ERROR: error_no=%04X, detail_no=%u\n", (HPDF_UINT)error_no, (HPDF_UINT)detail_no);
}

class PDFExporter : public Object
{
    GDCLASS(PDFExporter, Object);
  public:

    PDFExporter();
    virtual ~PDFExporter();

    void set_resolution(const Vector2i& resolution);
    Vector2i get_resolution() const;

    void export_recursive(Control* p_node);
    bool export_pdf(Object* p_node, const String& filename);

  protected:
    static void _bind_methods();

  private:

    void draw_text(const Vector2& position, const String& text);
    void draw_circle(const Vector2& position, double radius, const Color& color);
    void draw_rect(const Vector2& position, const Vector2& size, const Color& color);

    HPDF_Doc  pdf;
    HPDF_Page page;

};

String convert_string_to_filename(const String &path);

#endif // !HARU_H