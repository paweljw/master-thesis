ActionController::Renderers.add :yml do |object, options|
  self.content_type ||= Mime::YAML
  object.respond_to?(:to_yaml) ? object.to_yaml : object
end