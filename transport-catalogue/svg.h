#pragma once

#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <optional>
#include <variant>
#include <string_view>
#include <utility>
#include <deque>

namespace svg {
	struct Rgb {
		Rgb()
			: red(0)
			, green(0)
			, blue(0) {
		}
		Rgb(const uint8_t& r, const uint8_t& g, const uint8_t& b);

		uint8_t red;
		uint8_t green;
		uint8_t blue;
	};
	struct Rgba : public Rgb {
		Rgba()
			: Rgb()
			, opacity(1.0) {
		}
		Rgba(const uint8_t& r, const uint8_t& g, const uint8_t& b, const double& o);

		double opacity ;
	};

	struct Point {
		Point() = default;
		Point(const double& x, const double& y);

		double x = 0;
		double y = 0;
	};

	struct RenderContext {
		RenderContext(std::ostream& out);
		RenderContext(std::ostream& out, const int& indent_step, const int& indent = 0);

		RenderContext Indented() const;
		void RenderIndent() const;

		std::ostream& out;
		int indent_step = 0;
		int indent = 0;
	};

	class Object {
	public:

		void Render(const RenderContext& context) const;
		virtual ~Object() = default;

	private:

		virtual void RenderObject(const RenderContext& context) const = 0;
	};

	enum class StrokeLineCap {
		BUTT,
		ROUND,
		SQUARE,
	};

	enum class StrokeLineJoin {
		ARCS,
		BEVEL,
		MITER,
		MITER_CLIP,
		ROUND,
	};

	using Color = std::variant<std::monostate, std::string, Rgb, Rgba>;

	inline const Color NoneColor{std::monostate() };

	std::ostream& operator<<(std::ostream& out, const StrokeLineCap& line_cap);
	std::ostream& operator<<(std::ostream& out, const StrokeLineJoin& line_join);

	template <typename Owner>
	class PathProps {
	public:
		Owner& SetFillColor(const Color& color) {
			fill_color_ = std::move(color);
			return AsOwner();
		}
		Owner& SetStrokeColor(const Color& color) {
			stroke_color_ = std::move(color);
			return AsOwner();
		}
		Owner& SetStrokeWidth(const double& width) {
			stroke_width_ = width;
			return AsOwner();
		}
		Owner& SetStrokeLineCap(const StrokeLineCap& line_cap) {
			line_cap_ = line_cap;
			return AsOwner();
		}
		Owner& SetStrokeLineJoin(const StrokeLineJoin& line_join) {
			line_join_ = line_join;
			return AsOwner();
		}

	protected:

		~PathProps() = default;

		void RenderAttrs(std::ostream& out) const {
			using namespace std::literals;

			if (fill_color_) {
				out << " fill=\""sv;
				std::visit(ColorPrinter{ out }, *fill_color_);
				out << "\""sv;
			}
			if (stroke_color_) {
				out << " stroke=\""sv;
				std::visit(ColorPrinter{ out }, *stroke_color_);
				out << "\""sv;
			}
			if (stroke_width_) {
				out << " stroke-width=\""sv << *stroke_width_ << "\""sv;
			}
			if (line_cap_) {
				out << " stroke-linecap=\""sv << *line_cap_ << "\""sv;
			}
			if (line_join_) {
				out << " stroke-linejoin=\""sv << *line_join_ << "\""sv;
			}
		}

	private:

		std::optional<Color> fill_color_;
		std::optional<Color> stroke_color_;
		std::optional<double> stroke_width_;
		std::optional<StrokeLineCap> line_cap_;
		std::optional<StrokeLineJoin> line_join_;

		Owner& AsOwner() {
			return static_cast<Owner&>(*this);
		}

		struct ColorPrinter {
			std::ostream& out;

			void operator()(std::monostate) const {
				using namespace std::literals;
				out << "none"sv;
			}

			void operator()(const std::string_view& str) const {
				using namespace std::literals;
				out << str;
			}

			void operator()(const Rgb& rgb) {
				using namespace std::literals;
				out << "rgb("sv
					<< (int)rgb.red << ","sv
					<< (int)rgb.green << ","sv
					<< (int)rgb.blue << ")"sv;
			}

			void operator()(const Rgba& rgba) {
				using namespace std::literals;
				out << "rgba("sv
					<< (int)rgba.red << ","sv
					<< (int)rgba.green << ","sv
					<< (int)rgba.blue << ","sv
					<< rgba.opacity << ")"sv;
			}
		};
	};

	class Circle final : public Object, public PathProps<Circle> {
	public:

		Circle& SetCenter(const Point& center);
		Circle& SetRadius(const double& radius);

	private:

		Point center_;
		double radius_ = 1.0;

		void RenderObject(const RenderContext& context) const override;
	};

	class Polyline : public Object, public PathProps<Polyline>
	{
	public:

		Polyline& AddPoint(const Point& point);

	private:

		std::vector<Point> points_;

		void RenderObject(const RenderContext& context) const override;
	};

	class Text : public Object, public PathProps<Text>
	{
	public:

		Text& SetPosition(const Point& pos);
		Text& SetOffset(const Point& offset);
		Text& SetFontSize(const uint32_t& size);
		Text& SetFontFamily(const std::string& font_family);
		Text& SetFontWeight(const std::string& font_weight);
		Text& SetData(const std::string& data);

	private:

		Point position_;
		Point offset_;
		uint32_t font_size_ = 1;
		std::optional<std::string> font_family_;
		std::optional<std::string> font_weight_;
		std::string data_;

		void RenderObject(const RenderContext& context) const override;
	};

	class ObjectContainer {
	public:

		template<typename T>
		void Add(T obj) {
			objects_.emplace_back(std::make_shared<T>(std::move(obj)));
		}

		virtual void AddPtr(std::shared_ptr<Object>&& obj) = 0;

	protected:

		virtual ~ObjectContainer() = default;
		std::deque<std::shared_ptr<Object>> objects_;
	};

	class Drawable {
	public:

		virtual void Draw(ObjectContainer& container) const = 0;
		virtual ~Drawable() = default;
	};

	class Document final : public ObjectContainer {
	public:

		void AddPtr(std::shared_ptr<Object>&& obj) override;
		void Render(std::ostream& out) const;
	};

} // namespace svg


